#pragma once

#include <vector>

namespace dnxr {

class SignalStreamProcessor {
public:
    /// Applies denoising on raw neural samples before feature extraction.
    std::vector<float> denoise(const std::vector<float>& raw) const;
};

} // namespace dnxr
