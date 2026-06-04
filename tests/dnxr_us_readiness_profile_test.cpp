#include "readiness/USReadinessProfile.h"

#include <cassert>

int main() {
    dnxr::USReadinessProfile profile;
    assert(profile.country() == "United States");
    assert(profile.items().size() == 6U);
    assert(profile.blocksClinicalSubmission());
    profile.setItemComplete("IDE_PATHWAY_MEMO", true);
    profile.setItemComplete("SR_NSR_ASSUMPTION", true);
    assert(profile.missingItems().size() == 4U);
    return 0;
}
