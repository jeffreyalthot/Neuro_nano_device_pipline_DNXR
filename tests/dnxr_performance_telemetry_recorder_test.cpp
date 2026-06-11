#include "neural_chip/PerformanceTelemetryRecorder.h"
#include <cassert>
int main(){ dnxr::PerformanceTelemetryRecorder r; for(int i=1;i<=100;++i) r.record({double(i),1,2,3,4,5,6}); auto s=r.totalStats(); assert(s.p50Ms==50); assert(s.p95Ms==95); assert(s.p99Ms==99); assert(r.statsFor("commit").p95Ms==5); }
