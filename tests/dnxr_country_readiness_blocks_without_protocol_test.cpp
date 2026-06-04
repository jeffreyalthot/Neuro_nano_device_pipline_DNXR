#include "readiness/CanadaReadinessProfile.h"
#include "readiness/USReadinessProfile.h"

#include <cassert>

int main() {
    dnxr::CanadaReadinessProfile canada;
    dnxr::USReadinessProfile us;
    for (const auto& item : canada.items()) {
        if (item.id != "PROTOCOL_READINESS") {
            canada.setItemComplete(item.id, true);
        }
    }
    for (const auto& item : us.items()) {
        if (item.id != "MONITORING_PLAN") {
            us.setItemComplete(item.id, true);
        }
    }
    assert(canada.blocksClinicalSubmission());
    assert(us.blocksClinicalSubmission());
    return 0;
}
