#include "../src/hil/HILController.h"
#include <cassert>
int main(){ dnxr::FaultInjectionEngine f; f.inject({dnxr::HILFaultType::EmergencyStop,1,"e-stop"}); dnxr::LatencyInstrumentation l; l.record(1); dnxr::HILController c; auto r=c.runSession(dnxr::HardwarePhantomProfile{},f,l); assert(r.emergencyStopped); assert(r.safetyResponse); assert(r.recovered); return 0; }
