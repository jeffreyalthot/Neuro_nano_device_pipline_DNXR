// XRRenderer.h

#pragma once

#include <vector>
#include <cstdint>

namespace dnxr {

class XRRenderer {
public:
    XRRenderer();

    /// Presents the generated sensory frame to the XR output surface.
    void present(const std::vector<float>& frame) const;

    /// Metrics
    float getLastFrameEnergy() const;
    uint64_t getFrameCount() const;

    void setVsyncEnabled(bool enabled);
    bool isVsyncEnabled() const;

    void setFrameDropThreshold(float t);
    float getFrameDropThreshold() const;

private:
    float frameDropThreshold_;
    bool vsyncEnabled_;

    mutable float lastFrameEnergy_;
    mutable uint64_t frameCounter_;
};

} // namespace dnxr