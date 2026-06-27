#pragma once
#include "SafetyDecision.h"
#include <string>
#include <vector>
namespace dnxr { class SafetyAuditTrail { public: void append(const SafetyDecision& decision); std::string report() const; std::size_t size() const noexcept { return entries_.size(); } private: std::vector<std::string> entries_; }; }
