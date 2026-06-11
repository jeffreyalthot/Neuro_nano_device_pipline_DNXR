#include "neural_chip/DisabledModalityNeutralityValidator.h"
namespace dnxr {
DisabledModalityNeutralityResult DisabledModalityNeutralityValidator::validate(const FullDiveChipFrame& f, const ChipCapabilities& c) const {
    if (!c.canVisual && (f.visual.globalLuminance01 || f.visual.contrast01 || f.visual.motionFlow01 || f.visual.phospheneDensity01)) return {false,"disabled visual modality active"};
    if (!c.canAuditory && (f.auditory.loudness01 || f.auditory.lowBand01 || f.auditory.midBand01 || f.auditory.highBand01 || f.auditory.speechPriority01)) return {false,"disabled auditory modality active"};
    if (!c.canOlfactory && (f.olfactory.odorPresence01 || f.olfactory.smokeWarning01 || f.olfactory.chemicalWarning01)) return {false,"disabled olfactory modality active"};
    if (!c.canGustatory && (f.gustatory.sweet01 || f.gustatory.salty01 || f.gustatory.bitter01 || f.gustatory.umami01 || f.gustatory.swallowingConflictRisk01)) return {false,"disabled gustatory modality active"};
    if (!c.canVestibular && (f.vestibular.linearAcceleration01 || f.vestibular.angularVelocity01 || f.vestibular.motionSicknessRisk01)) return {false,"disabled vestibular modality active"};
    return {true,"accepted"};
}
}
