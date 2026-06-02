#include "../src/runtime/RuntimePipelineSinkRefs.h"

#include <cassert>
#include <cstdint>
#include <vector>

int main() {
    dnxr::RuntimePipelineSinks sinks;
    std::uint64_t states = 0U;
    std::uint64_t metrics = 0U;
    std::uint64_t replay = 0U;
    std::uint64_t drops = 0U;
    double replayTimestamp = 0.0;

    sinks.publishPipelineState = [&](const dnxr::RuntimePipelineFrame&) { ++states; };
    sinks.publishRuntimeMetrics = [&](const dnxr::RuntimeMetricsSnapshot&) { ++metrics; };
    sinks.recordReplayFrame = [&](const std::vector<float>& signals, double timestamp) {
        replay += signals.size();
        replayTimestamp = timestamp;
    };
    sinks.recordDroppedFrame = [&]() { ++drops; };

    dnxr::RuntimePipelineSinkRefs refs = dnxr::RuntimePipelineSinkRefs::fromSinks(sinks);
    assert(refs.publishPipelineState);
    assert(refs.publishRuntimeMetrics);
    assert(refs.recordReplayFrame);
    assert(refs.recordDroppedFrame);
    assert(!refs.triggerEmergency);

    dnxr::RuntimePipelineFrame frame;
    dnxr::RuntimeMetricsSnapshot snapshot;
    refs.publishPipelineState(frame);
    refs.publishRuntimeMetrics(snapshot);
    refs.recordReplayFrame(std::vector<float>{1.0f, 2.0f, 3.0f}, 42.5);
    refs.recordDroppedFrame();

    assert(states == 1U);
    assert(metrics == 1U);
    assert(replay == 3U);
    assert(replayTimestamp == 42.5);
    assert(drops == 1U);
    return 0;
}
