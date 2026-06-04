#include "../src/runtime/RuntimeLatencyProfiler.h"
#include <cassert>
int main(){ dnxr::RuntimeLatencyProfiler p; for(int i=1;i<=100;++i) p.record(i); auto r=p.report(); assert(r.average==50.5); assert(r.worst==100); assert(r.p95>=94); return 0; }
