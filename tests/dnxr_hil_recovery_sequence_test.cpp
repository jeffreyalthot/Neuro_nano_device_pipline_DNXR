#include "../src/hil/HILController.h"
#include <cassert>
int main(){ dnxr::FaultInjectionEngine f; f.inject({dnxr::HILFaultType::LatencyOverrun,2,"overrun"}); dnxr::LatencyInstrumentation l; l.record(30); dnxr::HILController c; auto r=c.runSession(dnxr::HardwarePhantomProfile{},f,l); assert(r.safetyResponse); assert(r.recovered); return 0; }
