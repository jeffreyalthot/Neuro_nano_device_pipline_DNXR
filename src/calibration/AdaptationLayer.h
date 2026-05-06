#pragma once

#include <vector>

namespace dnxr {

class AdaptationLayer {
public:
    /// Scales features using the active user sensitivity profile.
    std::vector<float> applyUserProfile(const std::vector<float>& vec, float gain) const;
};

} // namespace dnxr
