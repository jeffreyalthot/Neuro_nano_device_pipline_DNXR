#include "qms/RegulatoryAuthorizationEvidenceGate.h"
#include <cassert>
int main(){ dnxr::RegulatoryAuthorizationEvidenceGate gate("."); auto d=gate.evaluate(); assert(!d.allowHumanUse); assert(!d.allowStimulusDispatch); assert(!d.completeExternalPackage); assert(!d.missingArtifacts.empty()); }
