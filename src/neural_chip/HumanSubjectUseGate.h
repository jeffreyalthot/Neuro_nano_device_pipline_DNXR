#pragma once

#include <string>
#include <vector>

namespace dnxr {

enum class HumanSubjectUseStatus {
    BlockedSimulationOnly,
    PreclinicalReviewCandidate,
    BenchHilOnly,
    RegulatoryAuthorizationRequired,
    ExternalAuthorizationPresent
};

struct HumanSubjectUseDecision {
    HumanSubjectUseStatus status = HumanSubjectUseStatus::BlockedSimulationOnly;
    bool allowHumanUse = false;
    bool allowHardwareBench = false;
    bool allowHil = true;
    bool allowSimulation = true;
    bool allowStimulusDispatch = false;
    std::string reason;
};

struct HumanSubjectUseEvidence {
    bool simulationOnly = true;
    bool benchProfilePresent = false;
    bool hilEvidencePresent = false;
    bool regulatoryAuthorizationPresent = false;
    bool ethicsApprovalPresent = false;
    bool physicianSignoffPresent = false;
    bool hardwareBenchSignoffPresent = false;
};

class HumanSubjectUseGate {
public:
    HumanSubjectUseDecision evaluate(const HumanSubjectUseEvidence& evidence = {}) const;
};

const char* toString(HumanSubjectUseStatus status);

} // namespace dnxr
