#include "../src/neurotech/DeepImmersionPlatform.h"
#include "../src/immersion/ImmersionCore.h"
#include "../src/immersion/SensoryRenderer.h"
#include "../src/rendering/XRRenderer.h"

#include <cassert>
#include <cmath>
#include <string>
#include <vector>

namespace {

bool normalized(float value) {
    return std::isfinite(value) && value >= 0.0F && value <= 1.0F;
}

} // namespace

int main() {
    dnxr::DeepImmersionPlatform platform;
    platform.setConsentPolicy({true, true, true, "neural_chip_immersion_test"});
    platform.setInducedVector({0.03F, -0.01F, 0.02F});

    dnxr::BiometricFrame biometrics{};
    biometrics.heartRate = 72.0F;
    biometrics.respirationRate = 14.0F;
    biometrics.galvanicSkinResponse = 0.18F;
    biometrics.eyeFocus = 0.84F;
    biometrics.postureStability = 0.81F;
    biometrics.movementEnergy = 0.28F;

    const std::vector<float> neuralFrame = {
        0.12F, 0.24F, 0.31F, 0.46F, 0.28F, 0.19F, 0.35F, 0.42F,
        0.17F, 0.23F, 0.29F, 0.34F, 0.38F, 0.41F, 0.26F, 0.21F
    };

    const dnxr::CognitiveState cognitive =
        platform.runInferenceTick(neuralFrame, biometrics, "chip_to_immersion_test");
    const dnxr::XRAdaptationFrame adaptation =
        platform.runAdaptationTick(cognitive, biometrics);
    const dnxr::XRFullBodyCommand body =
        platform.runFullBodyControlTick(neuralFrame, cognitive);
    const dnxr::NeuroPipelineDiagnostics diagnostics =
        platform.lastDiagnostics();

    std::vector<float> callbackFrame;
    dnxr::ImmersionCore immersion;
    immersion.setOnFrameBuilt([&](const std::vector<float>& frame) {
        callbackFrame = frame;
    });
    const dnxr::NeuralSensoryFrame sensory =
        immersion.buildImmersiveFrameFromChip(
            cognitive,
            body,
            adaptation,
            diagnostics);

    const std::vector<float> channels = sensory.toVector();
    assert(channels.size() == 11U);
    for (float channel : channels) {
        assert(normalized(channel));
    }

    const dnxr::NeuralSensoryFrame cached = immersion.getLastNeuralFrame();
    assert(cached.toVector().size() == channels.size());

    const dnxr::VirtualViewFrame view = immersion.getLastVirtualViewFrame();
    const std::vector<float> viewChannels = view.toRenderVector();
    assert(viewChannels.size() == 16U);
    for (float channel : viewChannels) {
        assert(normalized(channel));
    }
    assert(!view.sceneName.empty());
    assert(!view.biomeName.empty());
    assert(!view.lightingMode.empty());
    assert(view.fieldOfViewDeg >= 70.0F && view.fieldOfViewDeg <= 110.0F);
    assert(view.nearPlane > 0.0F && view.farPlane > view.nearPlane);
    assert(view.stereoSeparation > 0.0F);

    const dnxr::VisualFieldFrame visualField = immersion.getLastVisualFieldFrame();
    const std::vector<float> visualFieldChannels = visualField.toRenderVector();
    assert(visualFieldChannels.size() == 16U);
    for (float channel : visualFieldChannels) {
        assert(normalized(channel));
    }
    assert(visualField.horizontalFovDeg >= 70.0F && visualField.horizontalFovDeg <= 122.0F);
    assert(visualField.verticalFovDeg >= 58.0F && visualField.verticalFovDeg <= 104.0F);
    assert(visualField.leftEyeYawOffsetDeg < 0.0F);
    assert(visualField.rightEyeYawOffsetDeg > 0.0F);
    assert(visualField.focalDepth > 0.0F);
    assert(visualField.nearDepthLayer <= visualField.midDepthLayer);
    assert(visualField.midDepthLayer <= visualField.farDepthLayer);

    const std::vector<float> composite = immersion.getLastCompositeRenderFrame();
    assert(composite.size() == channels.size() + viewChannels.size() + visualFieldChannels.size());
    assert(callbackFrame.size() == composite.size());
    for (float channel : composite) {
        assert(normalized(channel));
    }

    dnxr::XRRenderer xrRenderer;
    xrRenderer.setTargetFrameRate(90.0F);
    assert(xrRenderer.getTargetFrameRate() >= 89.0F && xrRenderer.getTargetFrameRate() <= 91.0F);
    const std::vector<float> rendererComposite = xrRenderer.composeImmersiveRenderFrame(channels, view, visualField);
    assert(rendererComposite.size() == composite.size());
    const dnxr::XRRenderValidation validation = xrRenderer.validateImmersiveRenderFrame(rendererComposite);
    assert(validation.accepted);
    assert(validation.expectedLayout);
    assert(validation.aboveEnergyThreshold);
    assert(validation.channelCount == dnxr::XRRenderer::kCompositeChannelCount);
    assert(validation.averageEnergy > 0.0F);
    xrRenderer.presentImmersiveFrame(channels, view, visualField);
    const dnxr::XRRenderValidation lastValidation = xrRenderer.getLastRenderValidation();
    assert(lastValidation.accepted);
    assert(xrRenderer.getFrameCount() == 1U);
    assert(xrRenderer.getDroppedFrameCount() == 0U);
    assert(std::string(xrRenderer.getLastRejectionReason()) == "none");
    assert(xrRenderer.getLastFrameEnergy() > 0.0F);
    const dnxr::XRFrameTimingMetrics timing = xrRenderer.getFrameTimingMetrics();
    assert(timing.targetFrameMs > 0.0F);
    assert(timing.lastFrameMs >= 0.0F);
    assert(timing.averageFrameMs >= 0.0F);
    assert(timing.jitterMs >= 0.0F);
    const dnxr::XRRenderQualityProfile fullQuality = xrRenderer.getAdaptiveQualityProfile();
    assert(!fullQuality.degradedQuality);
    assert(std::string(fullQuality.qualityMode) == "full_quality");
    assert(fullQuality.resolutionScale >= 0.99F);
    const std::vector<float> fullPresentation = xrRenderer.buildAdaptivePresentationFrame(rendererComposite);
    assert(fullPresentation == rendererComposite);
    const dnxr::XRRenderTelemetrySnapshot presentedTelemetry = xrRenderer.getTelemetrySnapshot();
    assert(presentedTelemetry.validation.accepted);
    assert(presentedTelemetry.frameCount == xrRenderer.getFrameCount());
    assert(presentedTelemetry.droppedFrameCount == 0U);
    assert(std::string(presentedTelemetry.rejectionReason) == "none");
    assert(presentedTelemetry.quality.resolutionScale == fullQuality.resolutionScale);
    const std::vector<float> presentedTelemetryVector = xrRenderer.getTelemetryVector();
    assert(presentedTelemetryVector.size() == dnxr::XRRenderer::kTelemetryChannelCount);
    for (float channel : presentedTelemetryVector) {
        assert(normalized(channel));
    }
    assert(presentedTelemetryVector[dnxr::XRRenderer::kTelemetryAcceptedChannel] == 1.0F);
    assert(presentedTelemetryVector[dnxr::XRRenderer::kTelemetryQualityModeChannel] == 0.0F);
    const dnxr::XRRenderQoSDecision stableDecision = xrRenderer.evaluateQoSDecision();
    assert(stableDecision.stabilityScore > 0.75F);
    assert(!stableDecision.throttleRecommended);
    assert(!stableDecision.fallbackRecommended);
    assert(std::string(stableDecision.qosMode) == "stable");

    std::vector<float> invalidComposite = rendererComposite;
    invalidComposite.push_back(0.5F);
    const dnxr::XRRenderValidation invalidValidation = xrRenderer.validateImmersiveRenderFrame(invalidComposite);
    assert(!invalidValidation.accepted);
    assert(!invalidValidation.expectedLayout);

    std::vector<float> quietComposite(dnxr::XRRenderer::kCompositeChannelCount, 0.0F);
    const dnxr::XRRenderValidation quietValidation = xrRenderer.validateImmersiveRenderFrame(quietComposite);
    assert(!quietValidation.accepted);
    assert(quietValidation.expectedLayout);
    assert(!quietValidation.aboveEnergyThreshold);
    xrRenderer.present(quietComposite);
    const dnxr::XRRenderQualityProfile recoveryQuality = xrRenderer.getAdaptiveQualityProfile();
    assert(recoveryQuality.degradedQuality);
    assert(recoveryQuality.reprojectionEnabled);
    assert(std::string(recoveryQuality.qualityMode) == "drop_recovery");
    assert(recoveryQuality.resolutionScale < fullQuality.resolutionScale);
    assert(recoveryQuality.recoveryFramesRemaining == 3U);
    const dnxr::XRRenderTelemetrySnapshot dropTelemetry = xrRenderer.getTelemetrySnapshot();
    assert(dropTelemetry.droppedFrameCount == 1U);
    assert(std::string(dropTelemetry.rejectionReason) == "below_energy_threshold");
    assert(dropTelemetry.quality.degradedQuality);
    const std::vector<float> recoveryTelemetryVector = xrRenderer.getTelemetryVector();
    assert(recoveryTelemetryVector.size() == presentedTelemetryVector.size());
    for (float channel : recoveryTelemetryVector) {
        assert(normalized(channel));
    }
    assert(recoveryTelemetryVector[dnxr::XRRenderer::kTelemetryDegradedQualityChannel] == 1.0F);
    assert(recoveryTelemetryVector[dnxr::XRRenderer::kTelemetryQualityModeChannel] == 1.0F);
    const dnxr::XRRenderQoSDecision fallbackDecision = xrRenderer.evaluateQoSDecision();
    assert(fallbackDecision.stabilityScore < stableDecision.stabilityScore);
    assert(fallbackDecision.throttleRecommended);
    assert(fallbackDecision.fallbackRecommended);
    assert(std::string(fallbackDecision.qosMode) == "fallback");
    const std::vector<float> degradedPresentation = xrRenderer.buildAdaptivePresentationFrame(rendererComposite);
    assert(degradedPresentation.size() == rendererComposite.size());
    for (float channel : degradedPresentation) {
        assert(normalized(channel));
    }
    assert(degradedPresentation[dnxr::XRRenderer::kFieldFovealRadiusChannel] >=
           rendererComposite[dnxr::XRRenderer::kFieldFovealRadiusChannel]);
    assert(degradedPresentation[dnxr::XRRenderer::kFieldPeripheralBlurChannel] >=
           rendererComposite[dnxr::XRRenderer::kFieldPeripheralBlurChannel]);
    assert(degradedPresentation[dnxr::XRRenderer::kViewFarPlaneChannel] <=
           rendererComposite[dnxr::XRRenderer::kViewFarPlaneChannel]);

    xrRenderer.presentImmersiveFrame(channels, view, visualField);
    const dnxr::XRRenderQualityProfile heldRecoveryQuality = xrRenderer.getAdaptiveQualityProfile();
    assert(heldRecoveryQuality.degradedQuality);
    assert(std::string(heldRecoveryQuality.qualityMode) == "drop_recovery");
    assert(heldRecoveryQuality.recoveryFramesRemaining == 2U);
    xrRenderer.presentImmersiveFrame(channels, view, visualField);
    xrRenderer.presentImmersiveFrame(channels, view, visualField);
    const dnxr::XRRenderQualityProfile settledQuality = xrRenderer.getAdaptiveQualityProfile();
    assert(!settledQuality.degradedQuality);
    assert(std::string(settledQuality.qualityMode) == "full_quality");
    assert(settledQuality.recoveryFramesRemaining == 0U);

    assert(immersion.getFrameCount() == 1U);
    assert(immersion.getLastIntensity() >= 0.0F);
    assert(!immersion.getAvatarPoseSummary().empty());

    return 0;
}
