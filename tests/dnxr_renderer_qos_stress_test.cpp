#include "../src/rendering/XRRenderer.h"
#include "../src/utils/Logger.h"
#include "../src/utils/RuntimeMetrics.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <limits>
#include <string>
#include <vector>

namespace {

bool normalized(float value) {
    return std::isfinite(value) && value >= 0.0f && value <= 1.0f;
}

void assertNormalizedVector(const std::vector<float>& values) {
    assert(!values.empty());
    for (float value : values) {
        assert(normalized(value));
    }
}

void assertQoSInRange(const dnxr::XRRenderQoSDecision& decision) {
    assert(std::isfinite(decision.stabilityScore));
    assert(decision.stabilityScore >= 0.0f);
    assert(decision.stabilityScore <= 1.0f);
    assert(decision.qosMode != nullptr);
    assert(std::string(decision.qosMode) == "stable" ||
           std::string(decision.qosMode) == "throttle" ||
           std::string(decision.qosMode) == "fallback");
}

} // namespace

int main() {
    dnxr::Logger::setMinLevel(dnxr::LogLevel::Fatal);

    dnxr::XRRenderer renderer;
    renderer.setVsyncEnabled(false);
    renderer.setTargetFrameRate(120.0f);
    renderer.setFrameDropThreshold(0.50f);

    const std::vector<float> validFrame(dnxr::XRRenderer::kCompositeChannelCount, 0.42f);
    const dnxr::XRRenderValidation baselineValidation = renderer.validateImmersiveRenderFrame(validFrame);
    assert(baselineValidation.accepted);
    assert(baselineValidation.expectedLayout);
    assert(baselineValidation.normalized);
    assert(baselineValidation.aboveEnergyThreshold);

    renderer.present(validFrame);
    assert(renderer.getFrameCount() == 1U);
    assert(renderer.getDroppedFrameCount() == 0U);
    assert(std::string(renderer.getLastRejectionReason()) == "none");
    assert(!renderer.getAdaptiveQualityProfile().degradedQuality);
    assertQoSInRange(renderer.evaluateQoSDecision());

    std::uint64_t expectedAcceptedFrames = 1U;
    std::uint64_t expectedDroppedFrames = 0U;
    std::uint32_t fallbackRecommendations = 0U;
    std::uint32_t throttleRecommendations = 0U;

    for (std::uint32_t cycle = 1U; cycle <= 512U; ++cycle) {
        if (cycle % 96U == 0U) {
            std::vector<float> invalidFrame = validFrame;
            invalidFrame[cycle % invalidFrame.size()] = std::numeric_limits<float>::quiet_NaN();
            const dnxr::XRRenderValidation invalidValidation = renderer.validateImmersiveRenderFrame(invalidFrame);
            assert(!invalidValidation.accepted);
            assert(!invalidValidation.finite);
            renderer.present(invalidFrame);
            ++expectedDroppedFrames;
            assert(std::string(renderer.getLastRejectionReason()) == "non_finite_or_empty");
        } else if (cycle % 40U == 0U) {
            const std::vector<float> quietFrame(dnxr::XRRenderer::kCompositeChannelCount, 0.0f);
            const dnxr::XRRenderValidation quietValidation = renderer.validateImmersiveRenderFrame(quietFrame);
            assert(!quietValidation.accepted);
            assert(quietValidation.expectedLayout);
            assert(!quietValidation.aboveEnergyThreshold);
            renderer.present(quietFrame);
            ++expectedDroppedFrames;
            assert(std::string(renderer.getLastRejectionReason()) == "below_energy_threshold");
        } else {
            if (cycle % 128U == 16U) {
                renderer.applyRuntimeLatencyHealth(dnxr::RuntimeLatencyHealth::Throttle);
                assert(std::string(renderer.getAdaptiveQualityProfile().qualityMode) == "timing_recovery");
            }
            if (cycle % 193U == 32U) {
                renderer.applyRuntimeLatencyHealth(dnxr::RuntimeLatencyHealth::Fallback);
                assert(std::string(renderer.getAdaptiveQualityProfile().qualityMode) == "drop_recovery");
            }
            renderer.present(validFrame);
            ++expectedAcceptedFrames;
            assert(std::string(renderer.getLastRejectionReason()) == "none");
        }

        const dnxr::XRRenderTelemetrySnapshot telemetry = renderer.getTelemetrySnapshot();
        assert(telemetry.frameCount == renderer.getFrameCount());
        assert(telemetry.droppedFrameCount == renderer.getDroppedFrameCount());
        assert(telemetry.quality.resolutionScale >= 0.0f);
        assert(telemetry.quality.resolutionScale <= 1.0f);
        assert(telemetry.quality.foveationStrength >= 0.0f);
        assert(telemetry.quality.foveationStrength <= 1.0f);
        assert(telemetry.quality.peripheralBlur >= 0.0f);
        assert(telemetry.quality.peripheralBlur <= 1.0f);
        assert(telemetry.timing.lastFrameMs >= 0.0f);
        assert(telemetry.timing.averageFrameMs >= 0.0f);
        assert(telemetry.timing.jitterMs >= 0.0f);

        const std::vector<float> telemetryVector = renderer.getTelemetryVector();
        assert(telemetryVector.size() == dnxr::XRRenderer::kTelemetryChannelCount);
        assertNormalizedVector(telemetryVector);

        const dnxr::XRRenderQoSDecision qos = renderer.evaluateQoSDecision();
        assertQoSInRange(qos);
        if (qos.throttleRecommended) {
            ++throttleRecommendations;
        }
        if (qos.fallbackRecommended) {
            ++fallbackRecommendations;
        }
    }

    assert(renderer.getFrameCount() == expectedAcceptedFrames);
    assert(renderer.getDroppedFrameCount() == expectedDroppedFrames);
    assert(expectedDroppedFrames > 0U);
    assert(throttleRecommendations > 0U);
    assert(fallbackRecommendations > 0U);

    renderer.applyRuntimeLatencyHealth(dnxr::RuntimeLatencyHealth::Fallback);
    const std::vector<float> degradedFrame = renderer.buildAdaptivePresentationFrame(validFrame);
    assert(degradedFrame.size() == validFrame.size());
    assertNormalizedVector(degradedFrame);
    assert(degradedFrame[dnxr::XRRenderer::kViewFarPlaneChannel] <= validFrame[dnxr::XRRenderer::kViewFarPlaneChannel]);
    assert(degradedFrame[dnxr::XRRenderer::kFieldFovealRadiusChannel] >= validFrame[dnxr::XRRenderer::kFieldFovealRadiusChannel]);
    assert(degradedFrame[dnxr::XRRenderer::kFieldPeripheralBlurChannel] >= validFrame[dnxr::XRRenderer::kFieldPeripheralBlurChannel]);

    for (std::uint32_t recovery = 0U; recovery < 6U; ++recovery) {
        renderer.present(validFrame);
    }
    const dnxr::XRRenderQualityProfile recoveredQuality = renderer.getAdaptiveQualityProfile();
    assert(!recoveredQuality.degradedQuality);
    assert(std::string(recoveredQuality.qualityMode) == "full_quality");
    assert(recoveredQuality.recoveryFramesRemaining == 0U);

    dnxr::Logger::resetToDefaultSinks();
    return 0;
}
