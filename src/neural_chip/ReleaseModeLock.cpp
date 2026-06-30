#include "neural_chip/ReleaseModeLock.h"

namespace dnxr {

ReleaseModeLockConfig ReleaseModeLock::lockedDefaults() const { return {}; }

ReleaseModeLockConfig ReleaseModeLock::apply(const ReleaseModeLockConfig& requested) const {
    ReleaseModeLockConfig locked = requested;
    locked.releaseMode = ReleaseMode::RND_MEDICAL_REVIEW_CANDIDATE;
    locked.simulationOnly = true;
    locked.humanUse = false;
    locked.allowStimulusDispatch = false;
    locked.chemicalChannelsDefault = false;
    locked.researchVisionDefault = false;
    locked.hardwareDispatchDefault = false;
    locked.benchOnly = true;
    locked.rawSensoryPayloadBlocked = true;
    locked.painMappingBlocked = true;
    if (locked.activeCeiling01 > 0.80F) locked.activeCeiling01 = 0.80F;
    if (locked.activeCeiling01 < 0.0F) locked.activeCeiling01 = 0.0F;
    return locked;
}

bool ReleaseModeLock::isCompliant(const ReleaseModeLockConfig& config) const {
    return config.releaseMode == ReleaseMode::RND_MEDICAL_REVIEW_CANDIDATE &&
           config.simulationOnly && !config.humanUse && !config.allowStimulusDispatch &&
           !config.chemicalChannelsDefault && !config.researchVisionDefault &&
           !config.hardwareDispatchDefault && config.benchOnly &&
           config.rawSensoryPayloadBlocked && config.painMappingBlocked &&
           config.activeCeiling01 >= 0.0F && config.activeCeiling01 <= 0.80F;
}

std::string ReleaseModeLock::explain(const ReleaseModeLockConfig& config) const {
    return isCompliant(config) ?
        "0.6.6 release mode locked to bench/HIL medical-review candidate with human use and stimulus dispatch disabled." :
        "0.6.6 release mode request violates bench-only no-human-use constraints.";
}

} // namespace dnxr
