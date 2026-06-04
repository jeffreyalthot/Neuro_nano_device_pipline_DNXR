#include "../src/hil/LatencyInstrumentation.h"
#include <cassert>
int main(){ dnxr::LatencyInstrumentation l; l.record(4); l.record(8); auto r=l.report(10); assert(r.withinBudget); assert(r.worstMs==8); return 0; }
