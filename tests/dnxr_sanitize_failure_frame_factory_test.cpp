#include "full_dive/chip_pipeline/SanitizeFailureFrameFactory.h"
#include "neural_chip/NeuralVectorValidator.h"
#include <cassert>
#include <cmath>
using namespace dnxr;
int main() {
    auto frame = SanitizeFailureFrameFactory::makeSanitizedFailureProbe(0, std::nan(""), "bad");
    assert(frame.frameId == 1);
    assert(frame.timestampMs == 0.0);
    assert(frame.motorIntent.stopIntent01 == 1.0f);
    assert(frame.realityAnchor.orientation01 == 1.0f);
    assert(NeuralVectorValidator::validate(frame).accepted);
    return 0;
}
