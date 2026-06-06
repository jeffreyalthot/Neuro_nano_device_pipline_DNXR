#include "../src/safety/HumanStopSignalMonitor.h"
#include <cassert>
int main(){ dnxr::HumanStopSignalMonitor m; dnxr::HumanStopSignals s; s.patientButton=true; auto d=m.evaluate(s); assert(d.shutdownLatched); assert(!d.reasons.empty()); return 0; }
