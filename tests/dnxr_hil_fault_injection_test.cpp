#include "../src/hil/FaultInjectionEngine.h"
#include <cassert>
int main(){ dnxr::FaultInjectionEngine f; f.inject({dnxr::HILFaultType::SignalLoss,1.0,"loss"}); assert(f.hasFault(dnxr::HILFaultType::SignalLoss)); assert(!f.hasFault(dnxr::HILFaultType::ReplayAttack)); return 0; }
