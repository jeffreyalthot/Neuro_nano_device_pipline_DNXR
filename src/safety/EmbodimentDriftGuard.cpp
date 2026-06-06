#include "EmbodimentDriftGuard.h"
namespace dnxr { EmbodimentDriftGuardResult EmbodimentDriftGuard::evaluate(const EmbodimentSignals& i) const { const bool ok = (!(i.avatarMismatch||i.limbInstability||i.wrongScale||i.asymmetricFeedback||i.delayedTouch)); return ok ? EmbodimentDriftGuardResult{true,"Allow","ok"} : EmbodimentDriftGuardResult{false,"EmergencyStop","embodiment_drift_detected"}; } }
