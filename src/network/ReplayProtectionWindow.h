#pragma once

#include <cstddef>
#include <cstdint>
#include <unordered_set>

namespace dnxr {

class ReplayProtectionWindow {
public:
    explicit ReplayProtectionWindow(std::size_t windowSize = 128U);

    bool accept(std::uint64_t counter);
    bool hasSeen(std::uint64_t counter) const;
    void reset();
    std::uint64_t highestAcceptedCounter() const;

private:
    void prune();

    std::size_t windowSize_;
    std::uint64_t highestAcceptedCounter_ = 0;
    std::unordered_set<std::uint64_t> seenCounters_;
};

} // namespace dnxr
