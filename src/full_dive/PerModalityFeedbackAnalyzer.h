#pragma once

#include "../hardware/ClosedLoopFeedbackFrame.h"

#include <array>
#include <string>

namespace dnxr {

enum class ClosedLoopFeedbackAction { Allow, Reduce, Hold, Anchor, Decompress, EmergencyStop };

struct PerModalityFeedbackAnalysis {
    std::array<float, 10> divergence{};
    ClosedLoopFeedbackAction action = ClosedLoopFeedbackAction::Allow;
    std::string reason = "feedback_modalities_coherent";
};

class PerModalityFeedbackAnalyzer {
public:
    PerModalityFeedbackAnalysis analyze(const ClosedLoopFeedbackFrame& expected,
                                        const ClosedLoopFeedbackFrame& neural,
                                        const ClosedLoopFeedbackFrame& physiological,
                                        const ClosedLoopFeedbackFrame& patientSignal) const;
};

} // namespace dnxr
