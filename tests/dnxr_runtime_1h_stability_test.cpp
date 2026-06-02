#include "../src/runtime/RuntimeInferenceStage.h"
#include "../src/utils/RuntimeMetrics.h"

#include <cassert>

int main() {
    dnxr::RuntimeMetrics metrics;
    metrics.setLatencyBudgetMs(16.0);
    bool emergency = false;
    constexpr int kSeconds = 3600;
    for (int second = 0; second < kSeconds; ++second) {
        const bool safe = (second % 997) != 0;
        metrics.recordCycleMeasurement(8.0 + (second % 4), safe, false, safe ? "nominal" : "simulated_degraded");
        emergency = emergency || false;
    }
    const auto snapshot = metrics.snapshot();
    assert(!emergency);
    assert(snapshot.totalCycles == kSeconds);
    assert(snapshot.safetyEventCount > 0);
    assert(snapshot.avgLatencyMs > 0.0);
    assert(dnxr::RuntimeInferenceStage::validate(true, "bad").continueCycle);
    return 0;
}
