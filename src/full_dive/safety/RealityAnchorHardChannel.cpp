#include "full_dive/safety/RealityAnchorHardChannel.h"

#include <algorithm>

namespace dnxr {

bool RealityAnchorHardChannel::validates(const NeuralRealityAnchorVector& anchor) const {
    return anchor.required &&
           anchor.orientation01 >= 0.70f &&
           anchor.identityContinuity01 >= 0.80f &&
           anchor.sessionBoundary01 >= 0.90f &&
           anchor.externalWorldRecall01 >= 0.80f;
}

FullDiveChipFrame RealityAnchorHardChannel::enforce(const FullDiveChipFrame& frame) const {
    if (validates(frame.realityAnchor)) {
        return frame;
    }

    FullDiveChipFrame out = frame;
    out.visual = NeuralVisualVector{};
    out.auditory = NeuralAuditoryVector{};
    out.touch = NeuralTouchVector{};
    out.proprioception = NeuralProprioceptionVector{};
    out.vestibular = NeuralVestibularVector{};
    out.olfactory = NeuralOlfactoryVector{};
    out.gustatory = NeuralGustatoryVector{};
    out.motorIntent.stopIntent01 = 1.0f;
    out.safety.aggregateRisk01 = std::max(out.safety.aggregateRisk01, 1.0f);
    return out;
}

} // namespace dnxr
