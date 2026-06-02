#include "../src/runtime/RuntimePipelineSinkRefs.h"

#include <atomic>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <new>
#include <vector>

namespace {
std::atomic<bool> g_countAllocations{false};
std::atomic<std::size_t> g_allocations{0U};
}

void* operator new(std::size_t size) {
    if (g_countAllocations.load(std::memory_order_relaxed)) {
        g_allocations.fetch_add(1U, std::memory_order_relaxed);
    }
    if (void* ptr = std::malloc(size)) {
        return ptr;
    }
    throw std::bad_alloc();
}

void operator delete(void* ptr) noexcept {
    std::free(ptr);
}

int main() {
    dnxr::RuntimePipelineSinks sinks;
    std::uint64_t states = 0U;
    std::uint64_t replay = 0U;
    sinks.publishPipelineState = [&](const dnxr::RuntimePipelineFrame&) { ++states; };
    sinks.recordReplayFrame = [&](const std::vector<float>& cleanSignals, double) { replay += cleanSignals.size(); };

    dnxr::RuntimePipelineFrame frame;
    const std::vector<float> signals{1.0f, 2.0f};

    g_allocations.store(0U, std::memory_order_relaxed);
    g_countAllocations.store(true, std::memory_order_relaxed);
    dnxr::RuntimePipelineSinkRefs refs = dnxr::RuntimePipelineSinkRefs::fromSinks(sinks);
    refs.publishPipelineState(frame);
    refs.recordReplayFrame(signals, 7.0);
    g_countAllocations.store(false, std::memory_order_relaxed);

    assert(states == 1U);
    assert(replay == 2U);
    assert(g_allocations.load(std::memory_order_relaxed) == 0U);
    return 0;
}
