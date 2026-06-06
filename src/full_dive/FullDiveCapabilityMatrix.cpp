#include "FullDiveCapabilityMatrix.h"

namespace dnxr {

const char* toString(FullDiveEvidenceLevel level) {
    switch (level) {
    case FullDiveEvidenceLevel::NotImplemented: return "NotImplemented";
    case FullDiveEvidenceLevel::MathematicalModelOnly: return "MathematicalModelOnly";
    case FullDiveEvidenceLevel::SoftwareSimulationOnly: return "SoftwareSimulationOnly";
    case FullDiveEvidenceLevel::ReplayTested: return "ReplayTested";
    case FullDiveEvidenceLevel::ClosedLoopSimulationTested: return "ClosedLoopSimulationTested";
    case FullDiveEvidenceLevel::HardwarePhantomTested: return "HardwarePhantomTested";
    case FullDiveEvidenceLevel::HILLatencyValidated: return "HILLatencyValidated";
    case FullDiveEvidenceLevel::HILFaultInjectionValidated: return "HILFaultInjectionValidated";
    case FullDiveEvidenceLevel::PreclinicalBenchValidated: return "PreclinicalBenchValidated";
    case FullDiveEvidenceLevel::IndependentReviewPassed: return "IndependentReviewPassed";
    case FullDiveEvidenceLevel::ClinicalProtocolApproved: return "ClinicalProtocolApproved";
    case FullDiveEvidenceLevel::HumanUseAuthorized: return "HumanUseAuthorized";
    case FullDiveEvidenceLevel::HumanSafetyEvidenceCollected: return "HumanSafetyEvidenceCollected";
    }
    return "Unknown";
}

std::vector<std::string> FullDiveCapabilityMatrix::requiredCapabilities() {
    return {"CAP-VISUAL", "CAP-AUDITORY", "CAP-HAPTIC", "CAP-PROPRIOCEPTIVE", "CAP-VESTIBULAR",
            "CAP-MOTOR-PREDICTION", "CAP-REALITY-ANCHOR", "CAP-EMERGENCY-EXIT",
            "CAP-COGNITIVE-REVERSIBILITY", "CAP-TEMPORAL-COHERENCE", "CAP-CALIBRATION-STABILITY",
            "CAP-SENSORY-EXPOSURE-CONTROL", "CAP-FULL-DIVE-DECOMPRESSION",
            "CAP-OLFACTORY", "CAP-GUSTATORY", "CAP-THERMAL", "CAP-BODY-SCHEMA",
            "CAP-CLOSED-LOOP-FEEDBACK", "CAP-PERCEPTION-ERROR-CORRECTION", "CAP-PATIENT-STOP-SIGNAL",
            "CAP-DEVICE-ATTESTATION", "CAP-MODALITY-PHASE-LOCK", "CAP-FIVE-SENSE-COHERENCE",
            "CAP-NO-PAIN-GUARANTEE", "CAP-LOCKED-IN-PREVENTION", "CAP-SCENE-RISK-GATING",
            "CAP-HARDWARE-CUTOFF", "CAP-SESSION-CALIBRATION"};
}

FullDiveCapabilityMatrix::FullDiveCapabilityMatrix() {
    for (const auto& id : requiredCapabilities()) {
        matrix_[id] = FullDiveCapabilityEvidence{id, FullDiveEvidenceLevel::NotImplemented, {}, false};
    }
}

void FullDiveCapabilityMatrix::setEvidence(const std::string& capabilityId, FullDiveEvidenceLevel level,
                                           const std::string& artifact, bool safetyControlLinked) {
    matrix_[capabilityId] = FullDiveCapabilityEvidence{capabilityId, level, artifact, safetyControlLinked};
}

FullDiveCapabilityEvidence FullDiveCapabilityMatrix::evidenceFor(const std::string& capabilityId) const {
    const auto it = matrix_.find(capabilityId);
    return it == matrix_.end() ? FullDiveCapabilityEvidence{capabilityId, FullDiveEvidenceLevel::NotImplemented, {}, false} : it->second;
}

std::vector<FullDiveCapabilityEvidence> FullDiveCapabilityMatrix::allEvidence() const {
    std::vector<FullDiveCapabilityEvidence> out;
    for (const auto& entry : matrix_) out.push_back(entry.second);
    return out;
}

std::vector<std::string> FullDiveCapabilityMatrix::capabilitiesBelow(FullDiveEvidenceLevel minimum) const {
    std::vector<std::string> gaps;
    for (const auto& entry : matrix_) {
        if (static_cast<int>(entry.second.level) < static_cast<int>(minimum) || !entry.second.safetyControlLinked) {
            gaps.push_back(entry.first);
        }
    }
    return gaps;
}

bool FullDiveCapabilityMatrix::allAtLeast(FullDiveEvidenceLevel minimum) const {
    return capabilitiesBelow(minimum).empty();
}

} // namespace dnxr
