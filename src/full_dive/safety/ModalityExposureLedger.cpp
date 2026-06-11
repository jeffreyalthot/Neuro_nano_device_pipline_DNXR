#include "full_dive/safety/ModalityExposureLedger.h"

#include <algorithm>

namespace dnxr {

float ModalityExposureLedger::clamp01(float value) {
    return std::max(0.0f, std::min(1.0f, value));
}

void ModalityExposureLedger::update(ModalityExposureState& state, float instantLoad01, float risk01) {
    state.instantLoad01 = clamp01(instantLoad01);
    state.risk01 = clamp01(risk01);
    state.rollingLoad01 = clamp01(state.rollingLoad01 * 0.98f + state.instantLoad01 * 0.02f);
    state.fatigue01 = clamp01(state.fatigue01 * 0.995f + state.risk01 * 0.005f);
}

FullDiveChipFrame ModalityExposureLedger::apply(const FullDiveChipFrame& frame) {
    FullDiveChipFrame out = frame;

    update(ledger_.visual, std::max(out.visual.globalLuminance01, out.visual.motionFlow01),
           std::max(out.visual.flickerRisk01, out.visual.visualFatigueRisk01));
    update(ledger_.auditory, out.auditory.loudness01, out.auditory.startleRisk01);
    update(ledger_.touch, std::max(out.touch.contactPressure01, out.touch.vibrationTexture01), out.touch.bodyCollision01);
    update(ledger_.proprioception, out.proprioception.avatarOwnership01, out.proprioception.bodySchemaMismatch01);
    update(ledger_.vestibular, std::max(out.vestibular.linearAcceleration01, out.vestibular.angularVelocity01),
           std::max(out.vestibular.motionSicknessRisk01, out.vestibular.visualVestibularMismatch01));
    update(ledger_.motor, out.motorIntent.gaitIntent01, std::max(out.motorIntent.spasmRisk01, out.motorIntent.fatigueRisk01));
    update(ledger_.olfactory, out.olfactory.odorPresence01,
           std::max(out.olfactory.nauseaRisk01, out.olfactory.trigeminalContaminationRisk01));
    update(ledger_.gustatory, std::max(out.gustatory.sweet01, out.gustatory.mouthfeel01),
           std::max(out.gustatory.nauseaRisk01, out.gustatory.swallowingConflictRisk01));

    if (ledger_.visual.fatigue01 > 0.75f) {
        out.visual.phospheneDensity01 *= 0.5f;
    }
    if (ledger_.auditory.fatigue01 > 0.75f) {
        out.auditory.loudness01 *= 0.5f;
    }
    if (ledger_.vestibular.risk01 > 0.60f) {
        out.vestibular.angularVelocity01 *= 0.4f;
    }
    ledger_.olfactory.hold = ledger_.olfactory.risk01 > 0.40f;
    ledger_.gustatory.hold = ledger_.gustatory.risk01 > 0.40f;
    if (ledger_.olfactory.hold || ledger_.gustatory.hold) {
        out.olfactory = NeuralOlfactoryVector{};
        out.gustatory = NeuralGustatoryVector{};
        out.safety.aggregateRisk01 = std::max(out.safety.aggregateRisk01, 0.40f);
    }
    if (out.motorIntent.spasmRisk01 > 0.50f) {
        out.motorIntent.stopIntent01 = 1.0f;
    }
    return out;
}

const FullDiveExposureLedger& ModalityExposureLedger::state() const {
    return ledger_;
}

void ModalityExposureLedger::reset() {
    ledger_ = {};
}

} // namespace dnxr
