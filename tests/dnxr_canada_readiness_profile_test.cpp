#include "readiness/CanadaReadinessProfile.h"

#include <cassert>

int main() {
    dnxr::CanadaReadinessProfile profile;
    assert(profile.country() == "Canada");
    assert(profile.items().size() == 6U);
    assert(profile.blocksClinicalSubmission());
    for (const auto& item : profile.items()) {
        profile.setItemComplete(item.id, true);
    }
    assert(!profile.blocksClinicalSubmission());
    assert(dnxr::toString(profile.status()) == "ReadyForQualifiedReview");
    return 0;
}
