#include "../src/runtime/RuntimeJitterAnalyzer.h"
#include <cassert>
int main(){ dnxr::RuntimeJitterAnalyzer j; j.recordPeriod(10); j.recordPeriod(10.5); j.recordPeriod(9.5); assert(j.within(1.0)); return 0; }
