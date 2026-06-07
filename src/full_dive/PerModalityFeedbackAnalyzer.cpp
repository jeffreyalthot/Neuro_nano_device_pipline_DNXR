#include "PerModalityFeedbackAnalyzer.h"

#include <algorithm>
#include <array>
#include <cmath>

namespace dnxr {
namespace {
std::array<float, 10> values(const ClosedLoopFeedbackFrame& f) {
    return {f.visualResponseConfidence, f.auditoryResponseConfidence, f.hapticResponseConfidence,
            f.thermalResponseConfidence, f.proprioceptiveResponseConfidence, f.vestibularResponseConfidence,
            f.olfactoryResponseConfidence, f.gustatoryResponseConfidence, f.realityAnchorResponseConfidence,
            f.patientStopResponseConfidence};
}
}

PerModalityFeedbackAnalysis PerModalityFeedbackAnalyzer::analyze(const ClosedLoopFeedbackFrame& expected,
                                                                 const ClosedLoopFeedbackFrame& neural,
                                                                 const ClosedLoopFeedbackFrame& physiological,
                                                                 const ClosedLoopFeedbackFrame& patientSignal) const {
    PerModalityFeedbackAnalysis out;
    const auto e = values(expected);
    const auto n = values(neural);
    const auto p = values(physiological);
    const auto ps = values(patientSignal);
    float maxDivergence = 0.0f;
    for (std::size_t i = 0; i < out.divergence.size(); ++i) {
        out.divergence[i] = std::max({std::fabs(e[i] - n[i]), std::fabs(n[i] - p[i]), std::fabs(n[i] - ps[i])});
        maxDivergence = std::max(maxDivergence, out.divergence[i]);
    }
    if (patientSignal.patientStopResponseConfidence < 0.5f) { out.action = ClosedLoopFeedbackAction::EmergencyStop; out.reason = "patient_stop_signal_uncertain"; return out; }
    if (neural.realityAnchorResponseConfidence < 0.35f) { out.action = ClosedLoopFeedbackAction::Anchor; out.reason = "reality_anchor_feedback_weak"; return out; }
    if (maxDivergence > 0.65f) { out.action = ClosedLoopFeedbackAction::Decompress; out.reason = "per_modality_feedback_divergence_high"; return out; }
    if (maxDivergence > 0.35f) { out.action = ClosedLoopFeedbackAction::Reduce; out.reason = "per_modality_feedback_divergence_moderate"; return out; }
    return out;
}

} // namespace dnxr
