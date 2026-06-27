#pragma once
#include "SafetyAuditTrail.h"
namespace dnxr { class EmergencyLatchController { public: void trigger(const SafetyDecision& decision); bool reset(const std::string& operatorId, const std::string& reason); bool latched() const noexcept { return latched_; } const SafetyAuditTrail& audit() const noexcept { return audit_; } private: bool latched_ = false; SafetyAuditTrail audit_; }; }
