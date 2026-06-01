#include "ReplayVerifier.h"

namespace dnxr {

ReplayVerificationResult ReplayVerifier::verify(
    const std::vector<RuntimeFrame>& expected,
    const std::vector<RuntimeFrame>& actual) {

    ReplayVerificationResult result;
    result.comparedFrames = expected.size() < actual.size() ? expected.size() : actual.size();

    if (expected.size() != actual.size()) {
        result.message = "frame_count_mismatch";
        result.firstMismatchIndex = result.comparedFrames;
        return result;
    }

    for (std::size_t i = 0; i < expected.size(); ++i) {
        const auto expectedHash = runtimeFrameHash(expected[i]);
        const auto actualHash = runtimeFrameHash(actual[i]);
        if (expectedHash != actualHash) {
            result.message = "frame_hash_mismatch";
            result.firstMismatchIndex = i;
            result.expectedHash = expectedHash;
            result.actualHash = actualHash;
            return result;
        }
    }

    result.deterministic = true;
    result.message = "deterministic_replay_match";
    return result;
}

} // namespace dnxr
