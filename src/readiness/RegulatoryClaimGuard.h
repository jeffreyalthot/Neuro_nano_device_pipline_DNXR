#pragma once

#include "readiness/ClinicalReadinessLevel.h"
#include "readiness/EvidenceRepository.h"
#include "full_dive/FullDiveCapabilityMatrix.h"

#include <string>
#include <vector>

namespace dnxr {

enum class RegulatoryClaim {
    SimulationOnlyPlatform,
    PreclinicalSoftwareReadinessPackage,
    HumanFullDiveImmersion,
    ClinicalTestReady,
    SafeForHumanUse,
    TherapeuticUse,
    ImplantReady,
    MedicalApproved
};

const char* toString(RegulatoryClaim claim);

struct RegulatoryClaimEvidenceContext {
    FullDiveEvidenceLevel fullDiveEvidence = FullDiveEvidenceLevel::NotImplemented;
    ClinicalReadinessLevel clinicalReadiness = ClinicalReadinessLevel::SimulationOnly;
    bool physicalHilQualified = false;
    bool preclinicalEvidenceComplete = false;
    bool clinicalEvidenceAvailable = false;
    bool authorizedInvestigationalUse = false;
    bool medicalApprovalGranted = false;
};

struct RegulatoryClaimDecision {
    bool allowed = false;
    std::string requiredEvidence;
    std::vector<std::string> blockers;
};

class RegulatoryClaimGuard {
public:
    RegulatoryClaimDecision evaluate(RegulatoryClaim claim, const RegulatoryClaimEvidenceContext& context) const;
    RegulatoryClaimDecision evaluate(RegulatoryClaim claim, const RegulatoryClaimEvidenceContext& context,
                                     const EvidenceRepository& evidence) const;

    static RegulatoryClaimEvidenceContext contextFromCapabilityMatrix(const FullDiveCapabilityMatrix& matrix,
                                                                       ClinicalReadinessLevel readiness);
};

} // namespace dnxr
