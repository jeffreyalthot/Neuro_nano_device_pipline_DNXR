#include "SafetyAuditTrail.h"
#include <sstream>
namespace dnxr { void SafetyAuditTrail::append(const SafetyDecision& decision){ entries_.push_back(decision.explain()); } std::string SafetyAuditTrail::report() const { std::ostringstream out; out << "safety_audit_entries=" << entries_.size(); for (const auto& e: entries_) out << "\n" << e; return out.str(); } }
