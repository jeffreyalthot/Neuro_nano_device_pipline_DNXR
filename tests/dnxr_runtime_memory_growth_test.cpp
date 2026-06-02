#include "../src/runtime/RuntimeSignalStage.h"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <vector>

int main() {
    constexpr std::uint32_t kCycles = 100000;
    std::vector<float> reusable(64, 0.0f);
    std::size_t maxCapacity = reusable.capacity();

    for (std::uint32_t cycle = 0; cycle < kCycles; ++cycle) {
        for (std::size_t i = 0; i < reusable.size(); ++i) {
            reusable[i] = static_cast<float>((cycle + i) % 128) / 128.0f;
        }
        maxCapacity = std::max(maxCapacity, reusable.capacity());
    }

    assert(reusable.capacity() == maxCapacity);
    assert(maxCapacity <= 64);
    assert(dnxr::RuntimeSignalStage::validate(true, "bad").continueCycle);
    return 0;
}
