#include "ExpectedPerceptionModel.h"

namespace dnxr {

ExpectedPerceptionSet ExpectedPerceptionModel::predictFixed(const FullDiveNeuralFrame& frame) const {
    ExpectedPerceptionSet out;
    frame.forEachChannel([&out](const NeuroChannelCommand& c) {
        ExpectedPerception perception{c.encodingModelId + ":" + c.targetRegion,
                                      c.intensity,
                                      0.10f + c.latencyBudgetMs / 1000.0f};
        out.try_push_back(perception);
    });
    return out;
}

std::vector<ExpectedPerception> ExpectedPerceptionModel::predict(const FullDiveNeuralFrame& frame) const {
    const auto fixed = predictFixed(frame);
    std::vector<ExpectedPerception> out;
    out.reserve(fixed.size());
    for (const auto& p : fixed) out.push_back(p);
    return out;
}

} // namespace dnxr
