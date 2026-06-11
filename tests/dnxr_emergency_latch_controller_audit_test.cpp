#include "neural_chip/EmergencyLatchController.h"
#include <cassert>
using namespace dnxr;
int main() {
    NeuralChipEmergencyLatch latch;
    EmergencyResetAuditTrail audit;
    PipelineResumeController resume;
    EmergencyLatchController c(latch, audit, resume);
    latch.trigger("test");
    EmergencyLatchResetRequest bad{1, "op", "", true, false};
    assert(!c.resetWithAudit(bad, 10.0));
    assert(latch.latched());
    EmergencyLatchResetRequest good{1, "op", "checked", true, true};
    assert(c.resetWithAudit(good, 20.0));
    assert(!latch.latched());
    assert(audit.records().size() == 2);
    assert(audit.records().back().resetAccepted);
    assert(!resume.normal());
    return 0;
}
