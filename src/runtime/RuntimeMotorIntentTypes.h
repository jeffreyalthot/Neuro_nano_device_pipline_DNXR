#pragma once

#include "../full_dive/motor_chip/MotorIntentCommand.h"

#include <cstdint>
#include <string>

namespace dnxr {

struct CausalMotorIntentModelIdentity {
    std::uint32_t schemaVersion = 1;
    std::string modelDigest = "sha256:dnxr-canonical-motor-intent-v1-nonhuman-fixture";
    bool signedManifest = true;
    bool deterministic = true;
    float minConfidence01 = 0.45f;
};

struct CausalMotorIntentStageResult {
    bool accepted = false;
    bool submittedToWorld = false;
    bool neutralHold = true;
    float signalQuality01 = 0.0f;
    float confidence01 = 0.0f;
    float uncertainty01 = 1.0f;
    std::string provenance = "not_evaluated";
    std::string reason = "not_evaluated";
    MotorIntentCommand command{};
};

} // namespace dnxr
