#include "NeuralIntentProtocol.h"

#include <algorithm>
#include <cmath>

namespace dnxr {

NeuralIntentVector NeuralIntentProtocol::decode(const std::vector<float>& neuralVector, float baseConfidence) const {
    NeuralIntentVector out;
    if (neuralVector.empty()) return out;

    const auto n = neuralVector.size();
    auto s = [&](std::size_t i) { return std::abs(neuralVector[i % n]); };

    const float grabScore = s(0) + s(1);
    const float rotateScore = s(4) + s(5);
    const float moveScore = s(2) + s(3);
    const float interruptScore = s(6);
    const float cancelScore = s(7);

    float best = grabScore;
    out.action = NeuralIntentAction::Grab;
    if (rotateScore > best) { best = rotateScore; out.action = NeuralIntentAction::Rotate; }
    if (moveScore > best) { best = moveScore; out.action = NeuralIntentAction::Move; }
    if (interruptScore > best) { best = interruptScore; out.action = NeuralIntentAction::Interrupt; }
    if (cancelScore > best) { best = cancelScore; out.action = NeuralIntentAction::Cancel; }

    out.intensity = std::clamp(best * 0.1f, 0.0f, 1.0f);
    out.confidence = std::clamp(baseConfidence * 0.7f + out.intensity * 0.3f, 0.0f, 1.0f);
    out.composable = (out.action == NeuralIntentAction::Move || out.action == NeuralIntentAction::Look || out.action == NeuralIntentAction::Rotate);
    return out;
}

bool NeuralIntentProtocol::isCompatibleSchema(const std::string& schemaVersion) const {
    return schemaVersion == "neuro.intent.embedding.v1";
}

} // namespace dnxr
