#include "readiness/CanadaReadinessProfile.h"

namespace dnxr {

CanadaReadinessProfile::CanadaReadinessProfile() : CountryReadinessProfile("Canada") {
    addItem("ITA_PATHWAY_MEMO", "ITA pathway memo and device classification assumptions reviewed by qualified regulatory owner");
    addItem("MANUFACTURER_IMPORTER_ROLE", "Manufacturer/importer role and responsibilities documented");
    addItem("REB_READINESS", "REB package readiness including protocol, consent, and investigator materials");
    addItem("INVESTIGATOR_PACKAGE", "Investigator package available and linked to preclinical evidence");
    addItem("PROTOCOL_READINESS", "Clinical protocol draft complete with stopping rules and adverse-event handling");
    addItem("MDR_ASSUMPTIONS", "Medical Devices Regulations assumptions documented and reviewed");
}

} // namespace dnxr
