#pragma once

#include "../runtime/RuntimeFrame.h"

#include <cstddef>
#include <string>
#include <vector>

namespace dnxr {

struct ReplayVerificationResult {
    bool deterministic = false;
    std::size_t comparedFrames = 0U;
    std::size_t firstMismatchIndex = 0U;
    std::string expectedHash;
    std::string actualHash;
    std::string message;
};

class ReplayVerifier {
public:
    static ReplayVerificationResult verify(
        const std::vector<RuntimeFrame>& expected,
        const std::vector<RuntimeFrame>& actual);
};

} // namespace dnxr
