#include "../src/clinical/ClinicalModeGate.h"
#include "../src/clinical/HumanSubjectSessionController.h"
#include <cassert>
int main(){ dnxr::ClinicalModeGate gate; auto d=gate.evaluateHumanUseRequest({}, 1000); assert(!d.humanUseAllowed); assert(!d.stimulusDispatchAllowed); dnxr::HumanSubjectSessionController c; dnxr::HumanSubjectSessionPrerequisites p; auto s=c.validateStart(p, {}, 1000); assert(!s.mayStart); return 0; }
