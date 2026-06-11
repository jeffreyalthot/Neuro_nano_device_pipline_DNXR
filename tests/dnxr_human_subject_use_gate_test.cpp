#include "neural_chip/HumanSubjectUseGate.h"
#include <cassert>
int main(){ dnxr::HumanSubjectUseGate gate; auto d=gate.evaluate(); assert(d.status==dnxr::HumanSubjectUseStatus::BlockedSimulationOnly); assert(!d.allowHumanUse); assert(!d.allowStimulusDispatch); dnxr::HumanSubjectUseEvidence e; e.benchProfilePresent=true; e.hilEvidencePresent=true; auto r=gate.evaluate(e); assert(r.status==dnxr::HumanSubjectUseStatus::PreclinicalReviewCandidate); assert(r.allowHardwareBench); assert(!r.allowHumanUse); }
