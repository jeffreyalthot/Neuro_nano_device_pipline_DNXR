#include "../src/core/NeuroRealtimeScheduler.h"
#include "../src/utils/DeterministicSessionReplay.h"
#include "../src/utils/SessionSafetyJournal.h"
#include "../src/utils/RuntimeMetrics.h"
#include "../src/utils/TimeSync.h"

#include <cassert>
#include <string>
#include <thread>

int main() {
    dnxr::NeuroRealtimeScheduler scheduler(20.0);
    scheduler.registerTask({"acquisition", 100, 3.0});

    dnxr::SchedulerTickReport report = scheduler.beginTick();
    auto t0 = std::chrono::steady_clock::now();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    scheduler.endTask("acquisition", t0, report);
    scheduler.endTick(report);

    assert(report.totalMs >= 1.0);
    assert(report.maxTaskMs >= 1.0);
    assert(report.jitterMs >= 0.0);

    dnxr::DeterministicSessionReplay replay;
    replay.record({{0.1f, 0.2f, 0.3f}, 1.0});
    replay.record({{0.4f, 0.5f, 0.6f}, 2.0});
    assert(replay.size() == 2);
    assert(replay.hasNext());
    const auto f1 = replay.next();
    const auto f2 = replay.next();
    assert(f1.timestamp == 1.0);
    assert(f2.timestamp == 2.0);
    assert(!replay.hasNext());
    replay.resetCursor();
    assert(replay.hasNext());

    dnxr::RuntimeMetrics metrics(10.0, 3);
    metrics.recordCycleMeasurement(4.0);
    metrics.recordCycleMeasurement(8.0);
    metrics.recordCycleMeasurement(12.0, false, true, "simulated_guardrail_trip");
    metrics.recordCycleMeasurement(20.0);
    const auto metricsSnapshot = metrics.snapshot();
    assert(metricsSnapshot.totalCycles == 4);
    assert(metricsSnapshot.lastLatencyMs == 20.0);
    assert(metricsSnapshot.maxLatencyMs == 20.0);
    assert(metricsSnapshot.avgLatencyMs == 11.0);
    assert(metricsSnapshot.jitterMs > 0.0);
    assert(metricsSnapshot.p95LatencyMs == 20.0);
    assert(metricsSnapshot.p99LatencyMs == 20.0);
    assert(metricsSnapshot.latencyBudgetMs == 10.0);
    assert(metricsSnapshot.latencyBudgetExceededCount == 2);
    assert(metricsSnapshot.windowBudgetExceededCount == 2);
    assert(metricsSnapshot.latencyHealthTransitionCount == 2);
    assert(metricsSnapshot.consecutiveNonStableCycles == 2);
    assert(metrics.latencyHealthTransitionCount() == 2);
    assert(metrics.consecutiveNonStableCycles() == 2);
    assert(metrics.safetyEventCount() == 1);
    assert(metricsSnapshot.safetyEventCount == 1);
    assert(metricsSnapshot.latencyHealth == dnxr::RuntimeLatencyHealth::Fallback);
    assert(metrics.latencyHealth() == dnxr::RuntimeLatencyHealth::Fallback);
    assert(std::string(dnxr::runtimeLatencyHealthToString(metrics.latencyHealth())) == "fallback");
    const std::string metricsPayload = dnxr::runtimeMetricsSnapshotToEventPayload(metricsSnapshot);
    assert(metricsPayload.find("metrics:latency_health=fallback") != std::string::npos);
    assert(metricsPayload.find("|total_cycles=4") != std::string::npos);
    assert(metricsPayload.find("|safety_events=1") != std::string::npos);
    assert(metricsPayload.find("|latency_budget_ms=10.000000") != std::string::npos);
    assert(metricsPayload.find("|budget_exceeded_total=2") != std::string::npos);
    assert(metricsPayload.find("|budget_exceeded=2") != std::string::npos);
    assert(metricsPayload.find("|health_transitions=2") != std::string::npos);
    assert(metricsPayload.find("|non_stable_streak=2") != std::string::npos);
    metrics.recordCycleMeasurement(4.0);
    metrics.recordCycleMeasurement(4.0);
    metrics.recordCycleMeasurement(4.0);
    const auto recoveredMetricsSnapshot = metrics.snapshot();
    assert(recoveredMetricsSnapshot.latencyHealth == dnxr::RuntimeLatencyHealth::Stable);
    assert(recoveredMetricsSnapshot.latencyHealthTransitionCount == 3);
    assert(recoveredMetricsSnapshot.consecutiveNonStableCycles == 0);
    assert(recoveredMetricsSnapshot.windowBudgetExceededCount == 0);
    metrics.setLatencyBudgetMs(3.0);
    const auto reconfiguredMetricsSnapshot = metrics.snapshot();
    assert(reconfiguredMetricsSnapshot.latencyBudgetMs == 3.0);
    assert(reconfiguredMetricsSnapshot.latencyHealth == dnxr::RuntimeLatencyHealth::Throttle);
    assert(reconfiguredMetricsSnapshot.latencyHealthTransitionCount == 4);
    assert(reconfiguredMetricsSnapshot.consecutiveNonStableCycles == 0);
    assert(reconfiguredMetricsSnapshot.windowBudgetExceededCount == 3);
    assert(std::string(dnxr::runtimeLatencyHealthToString(reconfiguredMetricsSnapshot.latencyHealth)) == "throttle");

    dnxr::TimeSync::reset();
    const auto ts1 = dnxr::TimeSync::nowMs();
    const auto ts2 = dnxr::TimeSync::nowMs();
    assert(ts2 > ts1);
    assert(dnxr::TimeSync::lastTimestamp() == ts2);

    dnxr::SessionSafetyJournal journal;
    const auto safetyEvent = journal.record("safety_decision", "nominal_runtime", 0.10f);
    const auto halEvent = journal.record("hal_write", "accepted_safe_stimulus", 0.20f);
    assert(safetyEvent.previousHash == "genesis");
    assert(halEvent.previousHash == safetyEvent.eventHash);
    assert(journal.events().size() == 2);
    assert(journal.lastEvent() != nullptr);
    assert(journal.latestHash() == halEvent.eventHash);
    assert(journal.verifyChain());
    journal.clear();
    assert(journal.events().empty());
    assert(journal.latestHash() == "genesis");

    return 0;
}
