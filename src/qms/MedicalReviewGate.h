#pragma once
#include <string>
#include <vector>

namespace dnxr {

enum class MedicalReviewStatus { Blocked, ReviewCandidate, EvidenceIncomplete, RequiresIndependentSignoff };

struct MedicalReviewEvidence {
    bool fullBuildPresent = false;
    bool ctestCompletePresent = false;
    bool hilP95P99Present = false;
    bool riskManagementFilePresent = false;
    bool traceabilityComplete = false;
    bool softwareSafetyCasePresent = false;
    bool cybersecuritySummaryPresent = false;
    bool sbomPresent = false;
    bool noOverstatedHumanClaims = false;
    bool noElectricalStimulationParams = false;
    bool noDirectHardwareDispatch = false;
    bool allHumanUseGatesBlocked = true;
    bool independentSignoffPresent = false;
};

struct MedicalReviewDecision { MedicalReviewStatus status = MedicalReviewStatus::Blocked; std::vector<std::string> gaps; bool allowHumanUse = false; };

class MedicalReviewGate { public: MedicalReviewDecision evaluate(const MedicalReviewEvidence& evidence) const; };
const char* toString(MedicalReviewStatus status);

} // namespace dnxr
