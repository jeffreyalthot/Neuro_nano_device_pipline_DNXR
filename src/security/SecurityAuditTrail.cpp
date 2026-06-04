#include "SecurityAuditTrail.h"
namespace dnxr { void SecurityAuditTrail::append(const ThreatEvent& e){events_.push_back(e);} std::vector<ThreatEvent> SecurityAuditTrail::events() const{return events_;} bool SecurityAuditTrail::containsCriticalEvent() const{for(const auto& e:events_) if(e.severity==ThreatSeverity::Critical) return true; return false;} }
