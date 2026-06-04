#include "hil/HILScenarioResult.h"

#include <cassert>

int main() {
    const auto stats = dnxr::computeHILLatencyStatistics({1.0, 2.0, 3.0, 4.0, 100.0});
    assert(stats.averageMs == 22.0);
    assert(stats.p95Ms > 80.0);
    assert(stats.p99Ms > 95.0);
    assert(stats.worstMs == 100.0);
    return 0;
}
