#pragma once

#include "../immersion/VirtualWorldState.h"
#include <vector>

namespace dnxr {

class XRRenderer {
public:
    /// Presents the generated sensory frame to the XR output surface.
    void present(const std::vector<float>& frame, const VirtualWorldState& world) const;

    /// Renders the persistent in-view quick menu entry button and optional safe-exit menu.
    void renderSafetyOverlay(const VirtualWorldState& world) const;
};

} // namespace dnxr
