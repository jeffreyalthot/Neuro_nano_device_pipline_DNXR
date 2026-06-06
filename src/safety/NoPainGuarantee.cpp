#include "NoPainGuarantee.h"
namespace dnxr { NoPainGuaranteeResult NoPainGuarantee::evaluate(const PainSafetyInput& i) const { const bool ok = (!(i.pain||i.burning||i.suffocation||i.crushing||i.fallPanic||i.forcedParalysis||i.lockedIn||i.fearInduction)); return ok ? NoPainGuaranteeResult{true,"Allow","ok"} : NoPainGuaranteeResult{false,"EmergencyStop","pain_related_perception_blocked"}; } }
