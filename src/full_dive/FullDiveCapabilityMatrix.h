#pragma once

#include <map>
#include <string>
#include <vector>

namespace dnxr {

enum class FullDiveEvidenceLevel {
    NotImplemented,
    MathematicalModelOnly,
    SoftwareSimulationOnly,
    ReplayTested,
    ClosedLoopSimulationTested,
    HardwarePhantomTested,
    HILLatencyValidated,
    HILFaultInjectionValidated,
    PreclinicalBenchValidated,
    IndependentReviewPassed,
    ClinicalProtocolApproved,
    HumanUseAuthorized,
    HumanSafetyEvidenceCollected,
    SimulatedOnly = SoftwareSimulationOnly,
    UnitTested = ReplayTested,
    IntegratedSimulation = ClosedLoopSimulationTested,
    HILValidated = HILLatencyValidated,
    PreclinicalEvidenceAvailable = PreclinicalBenchValidated,
    ClinicalEvidenceAvailable = HumanSafetyEvidenceCollected
};

const char* toString(FullDiveEvidenceLevel level);

struct FullDiveCapabilityEvidence {
    std::string capabilityId;
    FullDiveEvidenceLevel level = FullDiveEvidenceLevel::NotImplemented;
    std::string evidenceArtifact;
    bool safetyControlLinked = false;
};

class FullDiveCapabilityMatrix {
public:
    FullDiveCapabilityMatrix();
    void setEvidence(const std::string& capabilityId, FullDiveEvidenceLevel level,
                     const std::string& artifact, bool safetyControlLinked);
    FullDiveCapabilityEvidence evidenceFor(const std::string& capabilityId) const;
    std::vector<FullDiveCapabilityEvidence> allEvidence() const;
    std::vector<std::string> capabilitiesBelow(FullDiveEvidenceLevel minimum) const;
    bool allAtLeast(FullDiveEvidenceLevel minimum) const;
    static std::vector<std::string> requiredCapabilities();
private:
    std::map<std::string, FullDiveCapabilityEvidence> matrix_;
};

} // namespace dnxr
