#include "FullDiveCapabilityMatrix.h"

namespace dnxr {

const char* toString(FullDiveEvidenceLevel level) {
    switch (level) {
    case FullDiveEvidenceLevel::NotImplemented: return "NotImplemented";
    case FullDiveEvidenceLevel::SimulatedOnly: return "SimulatedOnly";
    case FullDiveEvidenceLevel::UnitTested: return "UnitTested";
    case FullDiveEvidenceLevel::IntegratedSimulation: return "IntegratedSimulation";
    case FullDiveEvidenceLevel::HILValidated: return "HILValidated";
    case FullDiveEvidenceLevel::PreclinicalEvidenceAvailable: return "PreclinicalEvidenceAvailable";
    case FullDiveEvidenceLevel::ClinicalEvidenceAvailable: return "ClinicalEvidenceAvailable";
    }
    return "Unknown";
}

std::vector<std::string> FullDiveCapabilityMatrix::requiredCapabilities() {
    return {"CAP-VISUAL", "CAP-AUDITORY", "CAP-HAPTIC", "CAP-PROPRIOCEPTIVE", "CAP-VESTIBULAR",
            "CAP-MOTOR-PREDICTION", "CAP-REALITY-ANCHOR", "CAP-EMERGENCY-EXIT",
            "CAP-COGNITIVE-REVERSIBILITY", "CAP-TEMPORAL-COHERENCE", "CAP-CALIBRATION-STABILITY",
            "CAP-SENSORY-EXPOSURE-CONTROL", "CAP-FULL-DIVE-DECOMPRESSION"};
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
