#pragma once
#include <string>
#include <vector>
namespace dnxr { enum class HardwareBenchReadinessStatus { EvidenceIncomplete, ReadyForMedicalReview }; struct HardwareBenchReadinessDecision { HardwareBenchReadinessStatus status=HardwareBenchReadinessStatus::EvidenceIncomplete; std::vector<std::string> missingReports; }; class HardwareBenchReadinessGate { public: explicit HardwareBenchReadinessGate(std::string root="."); HardwareBenchReadinessDecision evaluate() const; private: std::string root_; }; }
