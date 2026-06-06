#include "full_dive/PerceptionDivergenceDetector.h"
#include <cassert>
int main(){ dnxr::ClosedLoopFeedback f; f.available=false; auto r=dnxr::PerceptionDivergenceDetector().detect({},f,0.9f,100); assert(r.mustStop); return 0; }
