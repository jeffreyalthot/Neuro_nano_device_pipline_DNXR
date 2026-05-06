#pragma once

#include <vector>

namespace dnxr {

class XRRenderer {
public:
    /// Presents the generated sensory frame to the XR output surface.
    void present(const std::vector<float>& frame) const;
};

} // namespace dnxr
