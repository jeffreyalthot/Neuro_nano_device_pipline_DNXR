#pragma once
#include <string>
namespace dnxr { enum class SafetySeverity { Info, Warning, Critical }; struct SafetyDecision { bool accepted = true; std::string module; std::string rule; std::string expected; std::string received; SafetySeverity severity = SafetySeverity::Info; std::string action; std::string reproduce; std::string correction; std::string explain() const; }; }
