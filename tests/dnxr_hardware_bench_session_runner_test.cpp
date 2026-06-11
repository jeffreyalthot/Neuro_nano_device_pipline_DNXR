#include "hil/HardwareBenchSessionRunner.h"
#include <cassert>
int main(){ dnxr::HardwareBenchSessionRunner runner; auto m=runner.run(100); assert(!m.humanSubjectAllowed); assert(m.phantomLoopbackOnly); assert(m.transitionCount==100); assert(m.ackLatency.p95Ms>0); assert(m.replayWriteLatency.p99Ms>0); assert(m.toJson().find("phantom_loopback")!=std::string::npos); }
