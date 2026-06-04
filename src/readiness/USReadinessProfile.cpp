#include "readiness/USReadinessProfile.h"

namespace dnxr {

USReadinessProfile::USReadinessProfile() : CountryReadinessProfile("United States") {
    addItem("IDE_PATHWAY_MEMO", "IDE pathway memo and device description reviewed by qualified regulatory owner");
    addItem("SR_NSR_ASSUMPTION", "Significant-risk/non-significant-risk assumption documented");
    addItem("IRB_READINESS", "IRB package readiness including protocol, consent, and investigator materials");
    addItem("INFORMED_CONSENT", "Informed consent content drafted and linked to protocol risks");
    addItem("MONITORING_PLAN", "Monitoring plan, stopping rules, and adverse-event reporting process drafted");
    addItem("PART_812_MAPPING", "21 CFR Part 812 mapping prepared for qualified regulatory review");
}

} // namespace dnxr
