#include "qms/MedicalReviewGate.h"
#include <cassert>
int main(){ dnxr::MedicalReviewGate gate; dnxr::MedicalReviewEvidence e; auto missing=gate.evaluate(e); assert(missing.status==dnxr::MedicalReviewStatus::Blocked); e={true,true,true,true,true,true,true,true,true,true,true,true,false}; auto signoff=gate.evaluate(e); assert(signoff.status==dnxr::MedicalReviewStatus::RequiresIndependentSignoff); assert(!signoff.allowHumanUse); e.independentSignoffPresent=true; assert(gate.evaluate(e).status==dnxr::MedicalReviewStatus::ReviewCandidate); }
