#pragma once

#include "full_dive/motor_chip/AvatarKinematicRigController.h"
#include "neural_chip/NeuralChipFrame.h"

#include <cstdint>
#include <string>
#include <vector>

namespace dnxr {

enum class MotorIntentSafetyDisposition {
    Normal,
    ReducedMotion,
    SafetyPose,
    Hold,
    EmergencyStop
};

struct MotorIntentCommand {
    std::uint64_t frameId = 0;
    std::uint64_t validFromFrameId = 0;
    std::uint64_t validUntilFrameId = 0;
    double timestampMs = 0.0;
    float confidence01 = 0.0f;
    std::string provenance = "bci_motor_cortex";
    std::string patientBinding = "simulation_patient_bound";
    MotorIntentSafetyDisposition safetyDisposition = MotorIntentSafetyDisposition::Hold;
    NeuralMotorIntentVector intent;
    AvatarRigCommandFrame rig;
    std::string dispositionReason = "not_evaluated";
    std::uint64_t commandHash = 0U;
};

class ICanonicalMotorIntentSink {
public:
    virtual ~ICanonicalMotorIntentSink() = default;
    virtual bool submitMotorIntentCommand(const MotorIntentCommand& command) = 0;
};

class RecordingMotorIntentSink final : public ICanonicalMotorIntentSink {
public:
    bool submitMotorIntentCommand(const MotorIntentCommand& command) override;
    const std::vector<MotorIntentCommand>& commands() const;

private:
    std::vector<MotorIntentCommand> commands_;
};

} // namespace dnxr
