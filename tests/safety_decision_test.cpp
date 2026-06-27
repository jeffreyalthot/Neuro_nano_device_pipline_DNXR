#include "safety/EmergencyLatchController.h"
#include <cassert>
int main(){ dnxr::SafetyDecision d; d.accepted=false; d.module="test"; d.rule="emergency"; d.action="latch"; dnxr::EmergencyLatchController c; c.trigger(d); assert(c.latched()); assert(!c.reset("","missing")); assert(c.reset("operator","verified")); assert(!c.latched()); return 0; }
