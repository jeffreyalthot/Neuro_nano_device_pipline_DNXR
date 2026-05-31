// XRRenderer.h

#pragma once

#include <vector>
#include <cstdint>
#include <cstddef>

namespace dnxr {

struct VirtualViewFrame;
struct VisualFieldFrame;

struct XRFrameTimingMetrics {
    float targetFrameMs = 11.111f;
    float lastFrameMs = 0.0f;
    float averageFrameMs = 0.0f;
    float jitterMs = 0.0f;
    bool missedDeadline = false;
};

struct XRRenderValidation {
    std::size_t channelCount = 0U;
    bool finite = false;
    bool normalized = false;
    bool expectedLayout = false;
    bool aboveEnergyThreshold = false;
    bool accepted = false;
    float averageEnergy = 0.0f;
};

struct XRRenderQualityProfile {
    float resolutionScale = 1.0f;
    float foveationStrength = 0.35f;
    float peripheralBlur = 0.10f;
    bool reprojectionEnabled = false;
    bool degradedQuality = false;
    uint32_t recoveryFramesRemaining = 0U;
    const char* qualityMode = "full_quality";
};

struct XRRenderTelemetrySnapshot {
    XRRenderValidation validation{};
    XRFrameTimingMetrics timing{};
    XRRenderQualityProfile quality{};
    float lastFrameEnergy = 0.0f;
    uint64_t frameCount = 0U;
    uint64_t droppedFrameCount = 0U;
    const char* rejectionReason = "none";
};

struct XRRenderQoSDecision {
    float stabilityScore = 1.0f;
    bool throttleRecommended = false;
    bool fallbackRecommended = false;
    const char* qosMode = "stable";
};

class XRRenderer {
public:
    static constexpr std::size_t kSensoryChannelCount = 11U;
    static constexpr std::size_t kViewChannelCount = 16U;
    static constexpr std::size_t kVisualFieldChannelCount = 16U;
    static constexpr std::size_t kCompositeChannelCount =
        kSensoryChannelCount + kViewChannelCount + kVisualFieldChannelCount;
    static constexpr std::size_t kViewStartChannel = kSensoryChannelCount;
    static constexpr std::size_t kVisualFieldStartChannel = kSensoryChannelCount + kViewChannelCount;
    static constexpr std::size_t kViewFarPlaneChannel = kViewStartChannel + 6U;
    static constexpr std::size_t kViewParticleDensityChannel = kViewStartChannel + 13U;
    static constexpr std::size_t kViewFrameStabilityChannel = kViewStartChannel + 15U;
    static constexpr std::size_t kFieldFovealRadiusChannel = kVisualFieldStartChannel + 5U;
    static constexpr std::size_t kFieldPeripheralAwarenessChannel = kVisualFieldStartChannel + 6U;
    static constexpr std::size_t kFieldPeripheralBlurChannel = kVisualFieldStartChannel + 7U;
    static constexpr std::size_t kFieldVignetteChannel = kVisualFieldStartChannel + 8U;
    static constexpr std::size_t kFieldMotionComfortChannel = kVisualFieldStartChannel + 9U;
    static constexpr std::size_t kFieldRetinalStabilityChannel = kVisualFieldStartChannel + 15U;

    static constexpr std::size_t kTelemetryChannelCount = 20U;
    static constexpr std::size_t kTelemetryAcceptedChannel = 0U;
    static constexpr std::size_t kTelemetryFiniteChannel = 1U;
    static constexpr std::size_t kTelemetryNormalizedChannel = 2U;
    static constexpr std::size_t kTelemetryExpectedLayoutChannel = 3U;
    static constexpr std::size_t kTelemetryEnergyThresholdChannel = 4U;
    static constexpr std::size_t kTelemetryAverageEnergyChannel = 5U;
    static constexpr std::size_t kTelemetryLastFrameEnergyChannel = 6U;
    static constexpr std::size_t kTelemetryFrameCountChannel = 7U;
    static constexpr std::size_t kTelemetryDroppedFrameCountChannel = 8U;
    static constexpr std::size_t kTelemetryLastFrameMsChannel = 9U;
    static constexpr std::size_t kTelemetryAverageFrameMsChannel = 10U;
    static constexpr std::size_t kTelemetryJitterChannel = 11U;
    static constexpr std::size_t kTelemetryMissedDeadlineChannel = 12U;
    static constexpr std::size_t kTelemetryResolutionScaleChannel = 13U;
    static constexpr std::size_t kTelemetryFoveationChannel = 14U;
    static constexpr std::size_t kTelemetryPeripheralBlurChannel = 15U;
    static constexpr std::size_t kTelemetryReprojectionChannel = 16U;
    static constexpr std::size_t kTelemetryDegradedQualityChannel = 17U;
    static constexpr std::size_t kTelemetryRecoveryFramesChannel = 18U;
    static constexpr std::size_t kTelemetryQualityModeChannel = 19U;

    XRRenderer();

    /// Presents the generated sensory frame to the XR output surface.
    void present(const std::vector<float>& frame) const;

    /// Presents a camera/projection view of the virtual environment.
    void presentVirtualView(const VirtualViewFrame& view) const;

    /// Presents the binocular/foveal visual field from inside the environment.
    void presentVisualField(const VisualFieldFrame& field) const;

    /// Composes sensory + camera/projection + visual field channels into one XR frame.
    std::vector<float> composeImmersiveRenderFrame(
        const std::vector<float>& sensoryChannels,
        const VirtualViewFrame& view,
        const VisualFieldFrame& field) const;

    /// Applies the active adaptive quality profile to a validated composite frame.
    std::vector<float> buildAdaptivePresentationFrame(const std::vector<float>& compositeFrame) const;

    /// Presents one atomic immersive frame instead of separate sensory/view/field packets.
    void presentImmersiveFrame(
        const std::vector<float>& sensoryChannels,
        const VirtualViewFrame& view,
        const VisualFieldFrame& field) const;

    /// Validates that a composite frame is finite, normalized, and layout-compatible.
    XRRenderValidation validateImmersiveRenderFrame(const std::vector<float>& frame) const;
    XRRenderValidation getLastRenderValidation() const;

    /// Metrics
    float getLastFrameEnergy() const;
    uint64_t getFrameCount() const;
    uint64_t getDroppedFrameCount() const;
    XRFrameTimingMetrics getFrameTimingMetrics() const;
    XRRenderQualityProfile getAdaptiveQualityProfile() const;
    XRRenderTelemetrySnapshot getTelemetrySnapshot() const;
    std::vector<float> getTelemetryVector() const;
    XRRenderQoSDecision evaluateQoSDecision() const;

    void setVsyncEnabled(bool enabled);
    bool isVsyncEnabled() const;

    void setFrameDropThreshold(float t);
    float getFrameDropThreshold() const;
    void setTargetFrameRate(float framesPerSecond);
    float getTargetFrameRate() const;
    const char* getLastRejectionReason() const;

private:
    float frameDropThreshold_;
    bool vsyncEnabled_;
    float targetFrameMs_;

    mutable float lastFrameEnergy_;
    mutable uint64_t frameCounter_;
    mutable uint64_t droppedFrameCounter_;
    mutable XRRenderValidation lastValidation_;
    mutable XRFrameTimingMetrics timingMetrics_;
    mutable XRRenderQualityProfile adaptiveQuality_;
    mutable uint32_t qualityRecoveryFrames_;
    mutable const char* lastRejectionReason_;
};

} // namespace dnxr
