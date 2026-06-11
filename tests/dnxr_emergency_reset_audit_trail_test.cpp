#include "neural_chip/EmergencyResetAuditTrail.h"
#include <cassert>
int main(){ dnxr::EmergencyResetAuditTrail t; t.record({7,"op","bad",1.0,true,false,false}); t.record({7,"op","ok",2.0,true,true,true}); assert(t.records().size()==2); assert(!t.records()[0].resetAccepted); assert(t.records()[1].resetAccepted); }
