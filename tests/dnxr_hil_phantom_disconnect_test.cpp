#include "../src/hil/HILController.h"
#include <cassert>
int main(){ dnxr::HardwarePhantomProfile p; p.connected=false; dnxr::FaultInjectionEngine f; dnxr::LatencyInstrumentation l; l.record(1); dnxr::HILController c; auto r=c.runSession(p,f,l); assert(r.safetyResponse); assert(!r.passed); return 0; }
