#include "../src/security/SecurityAuditTrail.h"
#include <cassert>
int main(){ dnxr::SecurityAuditTrail t; t.append({"T-1","gateway",dnxr::ThreatSeverity::Critical,1,false,false}); assert(t.containsCriticalEvent()); assert(t.events().size()==1); return 0; }
