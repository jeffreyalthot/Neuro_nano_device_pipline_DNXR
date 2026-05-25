#pragma once

#include <string>
#include <vector>

namespace dnxr {

enum class NeuralIntentAction {
    None,
    Grab,
    Rotate,
    Interrupt,
    Cancel,
    Move,
    Look
};

struct NeuralIntentVector {
    std::string schemaVersion{"neuro.intent.embedding.v1"};
    NeuralIntentAction action{NeuralIntentAction::None};
    float intensity{0.0f};
    float confidence{0.0f};
    bool composable{false};
};

class NeuralIntentProtocol {
public:
    NeuralIntentVector decode(const std::vector<float>& neuralVector, float baseConfidence) const;
    bool isCompatibleSchema(const std::string& schemaVersion) const;
};

} // namespace dnxr
