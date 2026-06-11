#include "neural_chip/NeuralChipBus.h"
#include "neural_chip/NeuralVectorValidator.h"

#include <cassert>
#include <limits>

int main() {
    dnxr::FullDiveChipFrame frame;
    assert(dnxr::NeuralVectorValidator::validate(frame).accepted);

    auto invalid = frame;
    invalid.visual.flickerRisk01 = 1.01f;
    assert(!dnxr::NeuralVectorValidator::validate(invalid).accepted);

    invalid = frame;
    invalid.motorIntent.spasmRisk01 = std::numeric_limits<float>::quiet_NaN();
    assert(!dnxr::NeuralVectorValidator::validate(invalid).accepted);

    invalid = frame;
    invalid.safety.activeCeiling01 = 0.81f;
    assert(!dnxr::NeuralVectorValidator::validate(invalid).accepted);

    invalid = frame;
    invalid.safety.rawSensoryPayloadBlocked = false;
    assert(!dnxr::NeuralVectorValidator::validate(invalid).accepted);

    dnxr::NeuralChipBus bus;
    assert(bus.submitSensoryFrame(frame));
    assert(!bus.submitSensoryFrame(invalid));
    assert(bus.getHealthStatus().safeHoldActive);
    return 0;
}
