#include "neural_chip/NeuralChipSessionDigest.h"

namespace dnxr {

std::uint64_t NeuralChipSessionDigest::mix(std::uint64_t hash, std::uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

void NeuralChipSessionDigest::update(const NeuralChipReplayRecord& record) {
    digest_.rollingHash = mix(digest_.rollingHash, record.frameId);
    digest_.rollingHash = mix(digest_.rollingHash, static_cast<std::uint64_t>(record.timestampMs * 1000.0));
    digest_.rollingHash = mix(digest_.rollingHash, record.commitResult.accepted ? 1ULL : 0ULL);
    digest_.rollingHash = mix(digest_.rollingHash, record.commitResult.safetyHold ? 1ULL : 0ULL);
    ++digest_.frameCount;
    if (record.commitResult.accepted) ++digest_.acceptedCount;
    else ++digest_.rejectedCount;
}

const SessionDigest& NeuralChipSessionDigest::value() const { return digest_; }
void NeuralChipSessionDigest::reset() { digest_ = SessionDigest{}; }

} // namespace dnxr
