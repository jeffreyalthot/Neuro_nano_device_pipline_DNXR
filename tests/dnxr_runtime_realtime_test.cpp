#include "../src/core/NeuroRealtimeScheduler.h"
#include "../src/utils/DeterministicSessionReplay.h"
#include "../src/utils/SessionSafetyJournal.h"
#include "../src/utils/TimeSync.h"

#include <cassert>
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
