#include "neural_chip/FullDiveChipFrameDiff.h"
#include "neural_chip/NeuralVectorFieldRegistry.h"
#include "neural_chip/NeuralVectorValidator.h"
#include <cassert>
#include <string>
using namespace dnxr;
int main() {
    FullDiveChipFrame frame;
    std::size_t count = 0;
    NeuralVectorFieldRegistry::forEachAllFrameFields(frame, [&](const char*, const char*, float, bool){ ++count; });
    assert(count > 90);
    frame.gustatory.sweet01 = 2.0f;
    assert(!NeuralVectorValidator::validate(frame).accepted);
    FullDiveChipFrame before;
    auto deltas = FullDiveChipFrameDiff::compare(before, frame);
    bool found = false;
    for (const auto& d : deltas) if (std::string(d.modality) == "gustatory" && std::string(d.fieldPath) == "gustatory.sweet01") found = true;
    assert(found);
    return 0;
}
