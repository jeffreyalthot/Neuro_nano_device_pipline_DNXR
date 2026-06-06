#include "../src/runtime/ClinicalRealtimeLoop.h"
#include <cassert>
int main(){ dnxr::ClinicalRealtimeLoop loop; auto d=loop.runSafetyCycle({{"HardSafetyLoop",2.0,3.5}}); assert(d.emergencyLatched); auto ok=loop.runSafetyCycle({{"HardSafetyLoop",2.0,1.0}}); assert(!ok.emergencyLatched); return 0; }
