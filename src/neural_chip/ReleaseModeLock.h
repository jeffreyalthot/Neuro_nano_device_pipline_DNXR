#pragma once

#include <string>

namespace dnxr {

enum class ReleaseMode {
    RND_MEDICAL_REVIEW_CANDIDATE
};

struct ReleaseModeLockConfig {
    ReleaseMode releaseMode = ReleaseMode::RND_MEDICAL_REVIEW_CANDIDATE;
    bool simulationOnly = true;
    bool humanUse = false;
    bool allowStimulusDispatch = false;
    bool chemicalChannelsDefault = false;
    bool researchVisionDefault = false;
    bool hardwareDispatchDefault = false;
    bool benchOnly = true;
    bool rawSensoryPayloadBlocked = true;
    bool painMappingBlocked = true;
    float activeCeiling01 = 0.80F;
};

class ReleaseModeLock {
public:
    ReleaseModeLockConfig lockedDefaults() const;
    ReleaseModeLockConfig apply(const ReleaseModeLockConfig& requested) const;
    bool isCompliant(const ReleaseModeLockConfig& config) const;
    std::string explain(const ReleaseModeLockConfig& config) const;
};

} // namespace dnxr
