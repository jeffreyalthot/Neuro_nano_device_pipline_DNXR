#include "qms/ReleaseReadinessGate.h"
#include <cassert>
int main(){ dnxr::ReleaseReadinessGate gate; dnxr::ReleaseReadinessEvidence e; assert(gate.evaluate(e).status==dnxr::ReleaseReadinessStatus::Blocked); e={true,true,true,true,true,true,true,true,true,true,true,true,false}; assert(gate.evaluate(e).status==dnxr::ReleaseReadinessStatus::RNDReleaseCandidate); e.independentReviewPlanned=true; assert(gate.evaluate(e).status==dnxr::ReleaseReadinessStatus::MedicalReviewCandidate); }
