#pragma once
#include <cstdint>
#include <string>
namespace dnxr { enum class ThreatSeverity { Low, Medium, High, Critical }; struct ThreatEvent { std::string id; std::string source; ThreatSeverity severity=ThreatSeverity::Low; std::uint64_t counter=0; bool tamperDetected=false; bool replayDetected=false; }; }
