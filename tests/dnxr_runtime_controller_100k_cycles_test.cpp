#include "../src/runtime/RuntimeDeterminismReport.h"
#include <cassert>
int main(){ dnxr::RuntimeDeterminismReport r; r.latency.worst=5; r.jitterMs=0.5; r.allocationGrowth=0; assert(r.deterministicEnough(10,1)); return 0; }
