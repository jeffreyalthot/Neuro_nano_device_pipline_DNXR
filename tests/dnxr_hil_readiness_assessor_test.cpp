#include "../src/hil/HILReadinessAssessor.h"
#include <cassert>
int main(){ dnxr::HILEvidenceRecorder rec; dnxr::HILReadinessAssessor a; assert(!a.assess(rec).ready); rec.record({"fault injection",true,"qms/generated/hil.md"}); assert(a.assess(rec).ready); return 0; }
