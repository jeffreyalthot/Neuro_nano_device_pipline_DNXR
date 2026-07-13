#include "full_dive/motor_chip/MotorIntentCommand.h"

namespace dnxr {

bool RecordingMotorIntentSink::submitMotorIntentCommand(const MotorIntentCommand& command) {
    commands_.push_back(command);
    return true;
}

const std::vector<MotorIntentCommand>& RecordingMotorIntentSink::commands() const {
    return commands_;
}

} // namespace dnxr
