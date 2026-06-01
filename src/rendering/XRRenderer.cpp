// XRRenderer.cpp

#include "XRRenderer.h"

#include "../immersion/VirtualWorldState.h"
#include "../utils/Logger.h"
#include "../utils/RuntimeMetrics.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstring>
#include <mutex>
#include <numeric>
#include <thread>

namespace dnxr {

namespace {

std::mutex g_renderMutex;

bool isValidFrame(const std::vector<float>& frame) {
    if (frame.empty()) {
        return false;
    }

    return std::all_of(frame.begin(), frame.end(), [](float v) {
        return std::isfinite(v);
    });
}

float computeFrameEnergy(const std::vector<float>& frame) {
    float sum = 0.0f;
    for (float v : frame) {
        sum += std::abs(v);
    }
    return sum;
}

float qualityModeToken(const char* qualityMode) {
    if (qualityMode == nullptr || std::strcmp(qualityMode, "full_quality") == 0) {
        return 0.0f;
    }
    if (std::strcmp(qualityMode, "timing_recovery") == 0) {
        return 0.5f;
    }
    if (std::strcmp(qualityMode, "drop_recovery") == 0) {
        return 1.0f;
    }
    return 0.25f;
}

void updateTimingMetrics(XRFrameTimingMetrics& metrics, float elapsedMs) {
    metrics.lastFrameMs = elapsedMs;
    metrics.averageFrameMs = metrics.averageFrameMs <= 0.0f
        ? elapsedMs
        : metrics.averageFrameMs * 0.90f + elapsedMs * 0.10f;
    metrics.jitterMs = std::abs(elapsedMs - metrics.averageFrameMs);
    metrics.missedDeadline = elapsedMs > metrics.targetFrameMs;
}

void setDropRecoveryProfile(XRRenderQualityProfile& profile) {
    profile.resolutionScale = 0.75f;
    profile.foveationStrength = 0.75f;
    profile.peripheralBlur = 0.35f;
    profile.reprojectionEnabled = true;
    profile.degradedQuality = true;
    profile.qualityMode = "drop_recovery";
}

void setTimingRecoveryProfile(XRRenderQualityProfile& profile) {
    profile.resolutionScale = 0.82f;
    profile.foveationStrength = 0.62f;
    profile.peripheralBlur = 0.24f;
    profile.reprojectionEnabled = true;
    profile.degradedQuality = true;
    profile.qualityMode = "timing_recovery";
}

void setFullQualityProfile(XRRenderQualityProfile& profile) {
    profile.resolutionScale = 1.0f;
    profile.foveationStrength = 0.35f;
    profile.peripheralBlur = 0.10f;
    profile.reprojectionEnabled = false;
    profile.degradedQuality = false;
    profile.qualityMode = "full_quality";
}

void updateAdaptiveQualityProfile(
    XRRenderQualityProfile& profile,
    const XRFrameTimingMetrics& timing,
    bool acceptedFrame,
    uint32_t& recoveryFramesRemaining)
{
    if (!acceptedFrame) {
        recoveryFramesRemaining = 3U;
        setDropRecoveryProfile(profile);
        profile.recoveryFramesRemaining = recoveryFramesRemaining;
        return;
    }

    const float jitterBudgetMs = timing.targetFrameMs * 0.25f;
    if (timing.missedDeadline || timing.jitterMs > jitterBudgetMs) {
        recoveryFramesRemaining = std::max(recoveryFramesRemaining, 2U);
        setTimingRecoveryProfile(profile);
        profile.recoveryFramesRemaining = recoveryFramesRemaining;
        return;
    }

    if (recoveryFramesRemaining > 0U) {
        --recoveryFramesRemaining;
        if (recoveryFramesRemaining > 0U) {
            profile.recoveryFramesRemaining = recoveryFramesRemaining;
            return;
        }
    }

    setFullQualityProfile(profile);
    profile.recoveryFramesRemaining = 0U;
}

} // namespace

XRRenderer::XRRenderer()
    : frameDropThreshold_(0.001f),
      vsyncEnabled_(true),
      targetFrameMs_(1000.0f / 90.0f),
      lastFrameEnergy_(0.0f),
      frameCounter_(0),
      droppedFrameCounter_(0),
      lastValidation_{},
      timingMetrics_{},
      adaptiveQuality_{},
      qualityRecoveryFrames_(0U),
      lastRejectionReason_("none") {
    timingMetrics_.targetFrameMs = targetFrameMs_;
}

void XRRenderer::present(const std::vector<float>& frame) const {

    std::lock_guard<std::mutex> lock(g_renderMutex);
    const auto frameStart = std::chrono::steady_clock::now();

    if (!isValidFrame(frame)) {
        lastRejectionReason_ = "non_finite_or_empty";
        ++droppedFrameCounter_;
        updateAdaptiveQualityProfile(adaptiveQuality_, timingMetrics_, false, qualityRecoveryFrames_);
        Logger::warn("XRRenderer rejected invalid frame.");
        return;
    }

    float energy = computeFrameEnergy(frame);
    if (energy < frameDropThreshold_) {
        lastRejectionReason_ = "below_energy_threshold";
        ++droppedFrameCounter_;
        updateAdaptiveQualityProfile(adaptiveQuality_, timingMetrics_, false, qualityRecoveryFrames_);
        Logger::warn("XRRenderer dropped low-energy frame | energy=" + std::to_string(energy) +
                     " | threshold=" + std::to_string(frameDropThreshold_));
        return;
    }

    lastRejectionReason_ = "none";
    lastFrameEnergy_ = energy;
    ++frameCounter_;

    // Simulation pipeline de rendu. VSync can be disabled for low-latency tests.
    if (vsyncEnabled_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }

    const auto frameEnd = std::chrono::steady_clock::now();
    const float elapsedMs = std::chrono::duration<float, std::milli>(frameEnd - frameStart).count();
    updateTimingMetrics(timingMetrics_, elapsedMs);
    updateAdaptiveQualityProfile(adaptiveQuality_, timingMetrics_, true, qualityRecoveryFrames_);

    Logger::info(
        "XR frame presented | channels=" +
        std::to_string(frame.size()) +
        " | energy=" +
        std::to_string(energy) +
        " | frameId=" +
        std::to_string(frameCounter_) +
        " | frameMs=" +
        std::to_string(timingMetrics_.lastFrameMs));
}


void XRRenderer::presentVirtualView(const VirtualViewFrame& view) const {
    const std::vector<float> renderChannels = view.toRenderVector();
    present(renderChannels);
    Logger::info("XR virtual environment view presented | scene=" + view.sceneName +
                 " | biome=" + view.biomeName +
                 " | lighting=" + view.lightingMode +
                 " | fov=" + std::to_string(view.fieldOfViewDeg));
}


void XRRenderer::presentVisualField(const VisualFieldFrame& field) const {
    const std::vector<float> renderChannels = field.toRenderVector();
    present(renderChannels);
    Logger::info("XR visual field presented | hfov=" + std::to_string(field.horizontalFovDeg) +
                 " | vfov=" + std::to_string(field.verticalFovDeg) +
                 " | foveal=" + std::to_string(field.fovealRadius) +
                 " | peripheral=" + std::to_string(field.peripheralAwareness));
}


std::vector<float> XRRenderer::composeImmersiveRenderFrame(
    const std::vector<float>& sensoryChannels,
    const VirtualViewFrame& view,
    const VisualFieldFrame& field) const
{
    std::vector<float> frame;
    const std::vector<float> viewChannels = view.toRenderVector();
    const std::vector<float> fieldChannels = field.toRenderVector();
    frame.reserve(sensoryChannels.size() + viewChannels.size() + fieldChannels.size());
    frame.insert(frame.end(), sensoryChannels.begin(), sensoryChannels.end());
    frame.insert(frame.end(), viewChannels.begin(), viewChannels.end());
    frame.insert(frame.end(), fieldChannels.begin(), fieldChannels.end());
    return frame;
}

std::vector<float> XRRenderer::buildAdaptivePresentationFrame(const std::vector<float>& compositeFrame) const {
    std::vector<float> adjusted = compositeFrame;
    if (adjusted.size() != kCompositeChannelCount || !adaptiveQuality_.degradedQuality) {
        return adjusted;
    }

    adjusted[kViewFarPlaneChannel] = std::clamp(adjusted[kViewFarPlaneChannel] * adaptiveQuality_.resolutionScale, 0.0f, 1.0f);
    adjusted[kViewParticleDensityChannel] = std::clamp(adjusted[kViewParticleDensityChannel] * adaptiveQuality_.resolutionScale, 0.0f, 1.0f);
    adjusted[kViewFrameStabilityChannel] = std::clamp(adjusted[kViewFrameStabilityChannel] + 0.05f, 0.0f, 1.0f);

    adjusted[kFieldFovealRadiusChannel] = std::clamp(std::max(adjusted[kFieldFovealRadiusChannel], adaptiveQuality_.foveationStrength), 0.0f, 1.0f);
    adjusted[kFieldPeripheralAwarenessChannel] = std::clamp(adjusted[kFieldPeripheralAwarenessChannel] * adaptiveQuality_.resolutionScale, 0.0f, 1.0f);
    adjusted[kFieldPeripheralBlurChannel] = std::clamp(std::max(adjusted[kFieldPeripheralBlurChannel], adaptiveQuality_.peripheralBlur), 0.0f, 1.0f);
    adjusted[kFieldVignetteChannel] = std::clamp(std::max(adjusted[kFieldVignetteChannel], adaptiveQuality_.peripheralBlur * 0.5f), 0.0f, 1.0f);
    adjusted[kFieldMotionComfortChannel] = std::clamp(std::max(adjusted[kFieldMotionComfortChannel], 0.80f), 0.0f, 1.0f);
    if (adaptiveQuality_.reprojectionEnabled) {
        adjusted[kFieldRetinalStabilityChannel] = std::clamp(adjusted[kFieldRetinalStabilityChannel] + 0.10f, 0.0f, 1.0f);
    }

    return adjusted;
}

void XRRenderer::presentImmersiveFrame(
    const std::vector<float>& sensoryChannels,
    const VirtualViewFrame& view,
    const VisualFieldFrame& field) const
{
    const std::vector<float> frame = composeImmersiveRenderFrame(sensoryChannels, view, field);
    const XRRenderValidation validation = validateImmersiveRenderFrame(frame);
    if (!validation.accepted) {
        if (!validation.expectedLayout) lastRejectionReason_ = "unexpected_layout";
        else if (!validation.finite) lastRejectionReason_ = "non_finite_or_empty";
        else if (!validation.normalized) lastRejectionReason_ = "not_normalized";
        else if (!validation.aboveEnergyThreshold) lastRejectionReason_ = "below_energy_threshold";
        else lastRejectionReason_ = "validation_failed";
        ++droppedFrameCounter_;
        updateAdaptiveQualityProfile(adaptiveQuality_, timingMetrics_, false, qualityRecoveryFrames_);
        Logger::warn("XR immersive frame rejected | channels=" + std::to_string(validation.channelCount) +
                     " | expected=" + std::to_string(kCompositeChannelCount) +
                     " | reason=" + lastRejectionReason_);
        return;
    }
    const std::vector<float> presentationFrame = buildAdaptivePresentationFrame(frame);
    present(presentationFrame);
    Logger::info("XR immersive frame presented | sensory=" + std::to_string(sensoryChannels.size()) +
                 " | view=16 | visualField=16 | scene=" + view.sceneName +
                 " | hfov=" + std::to_string(field.horizontalFovDeg) +
                 " | avgEnergy=" + std::to_string(validation.averageEnergy));
}


XRRenderValidation XRRenderer::validateImmersiveRenderFrame(const std::vector<float>& frame) const {
    XRRenderValidation validation;
    validation.channelCount = frame.size();
    validation.expectedLayout = frame.size() == kCompositeChannelCount;
    validation.finite = isValidFrame(frame);
    validation.normalized = validation.finite && std::all_of(frame.begin(), frame.end(), [](float v) {
        return v >= 0.0f && v <= 1.0f;
    });
    if (validation.finite && !frame.empty()) {
        const float frameEnergy = computeFrameEnergy(frame);
        validation.averageEnergy = frameEnergy / static_cast<float>(frame.size());
        validation.aboveEnergyThreshold = frameEnergy >= frameDropThreshold_;
    } else {
        validation.averageEnergy = 0.0f;
        validation.aboveEnergyThreshold = false;
    }
    validation.accepted = validation.expectedLayout && validation.finite &&
                          validation.normalized && validation.aboveEnergyThreshold;
    lastValidation_ = validation;
    return validation;
}

XRRenderValidation XRRenderer::getLastRenderValidation() const {
    return lastValidation_;
}

float XRRenderer::getLastFrameEnergy() const {
    return lastFrameEnergy_;
}

uint64_t XRRenderer::getFrameCount() const {
    return frameCounter_;
}

uint64_t XRRenderer::getDroppedFrameCount() const {
    return droppedFrameCounter_;
}

XRFrameTimingMetrics XRRenderer::getFrameTimingMetrics() const {
    return timingMetrics_;
}

XRRenderQualityProfile XRRenderer::getAdaptiveQualityProfile() const {
    return adaptiveQuality_;
}

XRRenderTelemetrySnapshot XRRenderer::getTelemetrySnapshot() const {
    XRRenderTelemetrySnapshot snapshot;
    snapshot.validation = lastValidation_;
    snapshot.timing = timingMetrics_;
    snapshot.quality = adaptiveQuality_;
    snapshot.lastFrameEnergy = lastFrameEnergy_;
    snapshot.frameCount = frameCounter_;
    snapshot.droppedFrameCount = droppedFrameCounter_;
    snapshot.rejectionReason = lastRejectionReason_;
    return snapshot;
}

std::vector<float> XRRenderer::getTelemetryVector() const {
    const XRRenderTelemetrySnapshot snapshot = getTelemetrySnapshot();
    const float targetFrameMs = std::max(snapshot.timing.targetFrameMs, 0.001f);

    std::vector<float> telemetry(kTelemetryChannelCount, 0.0f);
    telemetry[kTelemetryAcceptedChannel] = snapshot.validation.accepted ? 1.0f : 0.0f;
    telemetry[kTelemetryFiniteChannel] = snapshot.validation.finite ? 1.0f : 0.0f;
    telemetry[kTelemetryNormalizedChannel] = snapshot.validation.normalized ? 1.0f : 0.0f;
    telemetry[kTelemetryExpectedLayoutChannel] = snapshot.validation.expectedLayout ? 1.0f : 0.0f;
    telemetry[kTelemetryEnergyThresholdChannel] = snapshot.validation.aboveEnergyThreshold ? 1.0f : 0.0f;
    telemetry[kTelemetryAverageEnergyChannel] = std::clamp(snapshot.validation.averageEnergy, 0.0f, 1.0f);
    telemetry[kTelemetryLastFrameEnergyChannel] = std::clamp(snapshot.lastFrameEnergy / static_cast<float>(kCompositeChannelCount), 0.0f, 1.0f);
    telemetry[kTelemetryFrameCountChannel] = std::clamp(static_cast<float>(snapshot.frameCount) / 1000.0f, 0.0f, 1.0f);
    telemetry[kTelemetryDroppedFrameCountChannel] = std::clamp(static_cast<float>(snapshot.droppedFrameCount) / 1000.0f, 0.0f, 1.0f);
    telemetry[kTelemetryLastFrameMsChannel] = std::clamp(snapshot.timing.lastFrameMs / targetFrameMs, 0.0f, 1.0f);
    telemetry[kTelemetryAverageFrameMsChannel] = std::clamp(snapshot.timing.averageFrameMs / targetFrameMs, 0.0f, 1.0f);
    telemetry[kTelemetryJitterChannel] = std::clamp(snapshot.timing.jitterMs / targetFrameMs, 0.0f, 1.0f);
    telemetry[kTelemetryMissedDeadlineChannel] = snapshot.timing.missedDeadline ? 1.0f : 0.0f;
    telemetry[kTelemetryResolutionScaleChannel] = std::clamp(snapshot.quality.resolutionScale, 0.0f, 1.0f);
    telemetry[kTelemetryFoveationChannel] = std::clamp(snapshot.quality.foveationStrength, 0.0f, 1.0f);
    telemetry[kTelemetryPeripheralBlurChannel] = std::clamp(snapshot.quality.peripheralBlur, 0.0f, 1.0f);
    telemetry[kTelemetryReprojectionChannel] = snapshot.quality.reprojectionEnabled ? 1.0f : 0.0f;
    telemetry[kTelemetryDegradedQualityChannel] = snapshot.quality.degradedQuality ? 1.0f : 0.0f;
    telemetry[kTelemetryRecoveryFramesChannel] = std::clamp(static_cast<float>(snapshot.quality.recoveryFramesRemaining) / 3.0f, 0.0f, 1.0f);
    telemetry[kTelemetryQualityModeChannel] = qualityModeToken(snapshot.quality.qualityMode);
    return telemetry;
}


void XRRenderer::applyRuntimeLatencyHealth(RuntimeLatencyHealth health) const {
    switch (health) {
    case RuntimeLatencyHealth::Fallback:
        qualityRecoveryFrames_ = std::max(qualityRecoveryFrames_, 3U);
        setDropRecoveryProfile(adaptiveQuality_);
        adaptiveQuality_.recoveryFramesRemaining = qualityRecoveryFrames_;
        return;
    case RuntimeLatencyHealth::Throttle:
        qualityRecoveryFrames_ = std::max(qualityRecoveryFrames_, 2U);
        setTimingRecoveryProfile(adaptiveQuality_);
        adaptiveQuality_.recoveryFramesRemaining = qualityRecoveryFrames_;
        return;
    case RuntimeLatencyHealth::Stable:
        if (qualityRecoveryFrames_ == 0U) {
            setFullQualityProfile(adaptiveQuality_);
            adaptiveQuality_.recoveryFramesRemaining = 0U;
        }
        return;
    }
}

XRRenderQoSDecision XRRenderer::evaluateQoSDecision() const {
    const std::vector<float> telemetry = getTelemetryVector();

    const float validationPenalty = 0.35f * (1.0f - telemetry[kTelemetryAcceptedChannel]);
    const float timingPenalty = 0.20f * telemetry[kTelemetryMissedDeadlineChannel] +
                                0.15f * telemetry[kTelemetryJitterChannel];
    const float recoveryPenalty = 0.20f * telemetry[kTelemetryDegradedQualityChannel] +
                                  0.10f * telemetry[kTelemetryRecoveryFramesChannel];
    const float dropPenalty = 0.10f * telemetry[kTelemetryDroppedFrameCountChannel];

    XRRenderQoSDecision decision;
    decision.stabilityScore = std::clamp(
        1.0f - validationPenalty - timingPenalty - recoveryPenalty - dropPenalty,
        0.0f,
        1.0f);
    decision.throttleRecommended = telemetry[kTelemetryMissedDeadlineChannel] > 0.0f ||
                                   telemetry[kTelemetryDegradedQualityChannel] > 0.0f;
    decision.fallbackRecommended = telemetry[kTelemetryAcceptedChannel] < 1.0f ||
                                   telemetry[kTelemetryQualityModeChannel] >= 1.0f;

    if (decision.fallbackRecommended) {
        decision.qosMode = "fallback";
    } else if (decision.throttleRecommended) {
        decision.qosMode = "throttle";
    } else {
        decision.qosMode = "stable";
    }

    return decision;
}

void XRRenderer::setVsyncEnabled(bool enabled) {
    vsyncEnabled_ = enabled;
}

bool XRRenderer::isVsyncEnabled() const {
    return vsyncEnabled_;
}

void XRRenderer::setFrameDropThreshold(float t) {
    frameDropThreshold_ = std::max(0.0f, t);
}

float XRRenderer::getFrameDropThreshold() const {
    return frameDropThreshold_;
}

void XRRenderer::setTargetFrameRate(float framesPerSecond) {
    const float safeFps = std::clamp(framesPerSecond, 1.0f, 240.0f);
    targetFrameMs_ = 1000.0f / safeFps;
    timingMetrics_.targetFrameMs = targetFrameMs_;
}

float XRRenderer::getTargetFrameRate() const {
    return 1000.0f / targetFrameMs_;
}

const char* XRRenderer::getLastRejectionReason() const {
    return lastRejectionReason_;
}

} // namespace dnxr
