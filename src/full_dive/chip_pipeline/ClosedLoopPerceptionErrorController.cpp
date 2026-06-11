#include "full_dive/chip_pipeline/ClosedLoopPerceptionErrorController.h"

#include <algorithm>
#include <cmath>

namespace dnxr {

float ClosedLoopPerceptionErrorController::clamp01(float value) { return std::max(0.0f, std::min(1.0f, value)); }

PerModalityPerceptionError ClosedLoopPerceptionErrorController::analyze(const FullDiveChipFrame& sentFrame, const ChipReadFrame& feedback) const {
    PerModalityPerceptionError out;
    const float qualityError = 1.0f - clamp01(feedback.signalQuality01);
    out.visualError01 = clamp01(std::max(sentFrame.visual.flickerRisk01, qualityError * sentFrame.visual.motionFlow01));
    out.auditoryError01 = clamp01(std::max(sentFrame.auditory.startleRisk01, qualityError * sentFrame.auditory.loudness01));
    out.touchError01 = clamp01(qualityError * sentFrame.touch.contactPressure01);
    out.proprioceptionError01 = clamp01(sentFrame.proprioception.bodySchemaMismatch01);
    out.vestibularError01 = clamp01(std::max(sentFrame.vestibular.motionSicknessRisk01, sentFrame.vestibular.visualVestibularMismatch01));
    out.olfactoryError01 = clamp01(sentFrame.olfactory.nauseaRisk01);
    out.gustatoryError01 = clamp01(sentFrame.gustatory.nauseaRisk01);
    out.motorError01 = clamp01(std::fabs(sentFrame.motorIntent.confidence01 - feedback.signalQuality01));
    return out;
}

ClosedLoopCorrection ClosedLoopPerceptionErrorController::decide(const PerModalityPerceptionError& error) {
    ClosedLoopCorrection out;
    auto gain = [](float err) { return std::max(0.0f, 1.0f - err * 0.75f); };
    out.visualGain01 = gain(error.visualError01);
    out.auditoryGain01 = gain(error.auditoryError01);
    out.touchGain01 = gain(error.touchError01);
    out.vestibularGain01 = gain(error.vestibularError01);
    out.chemicalGain01 = std::max(0.0f, 0.25f - std::max(error.olfactoryError01, error.gustatoryError01));
    const float currentMax = std::max({error.visualError01, error.auditoryError01, error.touchError01, error.proprioceptionError01,
                                       error.vestibularError01, error.olfactoryError01, error.gustatoryError01, error.motorError01});
    const float previousMax = std::max({previous_.visualError01, previous_.auditoryError01, previous_.touchError01, previous_.proprioceptionError01,
                                        previous_.vestibularError01, previous_.olfactoryError01, previous_.gustatoryError01, previous_.motorError01});
    out.requireHold = currentMax > 0.85f || (currentMax > previousMax && currentMax > 0.65f);
    previous_ = error;
    return out;
}

FullDiveChipFrame ClosedLoopPerceptionErrorController::applyCorrection(const FullDiveChipFrame& frame, const ClosedLoopCorrection& correction) const {
    FullDiveChipFrame out = frame;
    out.visual.globalLuminance01 *= correction.visualGain01; out.visual.motionFlow01 *= correction.visualGain01; out.visual.phospheneDensity01 *= correction.visualGain01;
    out.auditory.loudness01 *= correction.auditoryGain01;
    out.touch.contactPressure01 *= correction.touchGain01; out.touch.vibrationTexture01 *= correction.touchGain01;
    out.vestibular.linearAcceleration01 *= correction.vestibularGain01; out.vestibular.angularVelocity01 *= correction.vestibularGain01;
    out.olfactory.odorPresence01 *= correction.chemicalGain01; out.gustatory.sweet01 *= correction.chemicalGain01;
    if (correction.requireHold) {
        out.safety.aggregateRisk01 = 1.0f;
        out.motorIntent.stopIntent01 = 1.0f;
        out.realityAnchor.externalWorldRecall01 = 1.0f;
    }
    return out;
}

void ClosedLoopPerceptionErrorController::reset() { previous_ = {}; }

} // namespace dnxr
