#include "../src/runtime/RuntimeFullDiveStage.h"

#include <cassert>

int main() {
    dnxr::RuntimePipelineFrame frame;
    frame.sensoryFrame = {1.0f, 0.5f};
    frame.fusedSafetyDecision.reason = "nominal";

    bool updated = false;
    auto reduced = dnxr::RuntimeFullDiveStage::execute(frame, dnxr::RuntimeFullDiveStage::Input{
        [&updated](const dnxr::FullDiveRuntimeInput&) { updated = true; },
        []() {
            dnxr::FullDiveRuntimeSnapshot snapshot;
            snapshot.intervention = dnxr::FullDiveInterventionLevel::ReduceIntensity;
            snapshot.severity = 0.5f;
            snapshot.traceTag = "full_dive:reduce";
            return snapshot;
        },
        [](const dnxr::FullDiveRuntimeSnapshot&, const dnxr::SafetyDecision& decision) { return decision; }
    });

    assert(updated);
    assert(reduced.continueCycle);
    assert(reduced.sensoryFrame[0] == 0.5f);

    auto stopped = dnxr::RuntimeFullDiveStage::execute(frame, dnxr::RuntimeFullDiveStage::Input{
        [](const dnxr::FullDiveRuntimeInput&) {},
        []() {
            dnxr::FullDiveRuntimeSnapshot snapshot;
            snapshot.intervention = dnxr::FullDiveInterventionLevel::EmergencyStop;
            snapshot.reason = "full_dive_emergency";
            snapshot.traceTag = "full_dive:emergency";
            return snapshot;
        },
        [](const dnxr::FullDiveRuntimeSnapshot&, const dnxr::SafetyDecision& decision) { return decision; }
    });
    assert(!stopped.continueCycle);
    assert(stopped.emergencyActive);
    return 0;
}
