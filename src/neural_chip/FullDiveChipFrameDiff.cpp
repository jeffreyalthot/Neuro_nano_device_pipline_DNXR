#include "neural_chip/FullDiveChipFrameDiff.h"
#include "neural_chip/NeuralVectorFieldRegistry.h"

#include <cmath>
#include <string>
#include <vector>

namespace dnxr {

std::vector<FrameFieldDelta> FullDiveChipFrameDiff::compare(const FullDiveChipFrame& before, const FullDiveChipFrame& after, float epsilon01) {
    std::vector<FrameFieldDelta> deltas;
    std::vector<NeuralVectorFieldView> beforeFields;
    static thread_local std::vector<std::string> paths;
    paths.clear();
    NeuralVectorFieldRegistry::forEachAllFrameFields(before, [&](const char* modality, const char* field, float value, bool neutral) {
        beforeFields.push_back({modality, field, value, neutral});
    });
    paths.reserve(beforeFields.size());
    std::size_t index = 0;
    NeuralVectorFieldRegistry::forEachAllFrameFields(after, [&](const char* modality, const char* field, float value, bool) {
        if (index >= beforeFields.size()) return;
        const float delta = value - beforeFields[index].value01;
        if (std::fabs(delta) > epsilon01) {
            paths.emplace_back(std::string(modality) + "." + field);
            deltas.push_back({modality, paths.back().c_str(), beforeFields[index].value01, value, delta});
        }
        ++index;
    });
    return deltas;
}

} // namespace dnxr
