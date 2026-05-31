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
    Look,
    Compose
};

struct NeuralIntentVector {
    std::string schemaVersion{"neuro.intent.embedding.v1.1"};
    NeuralIntentAction action{NeuralIntentAction::None};
    std::string actionToken{"none"};
    float intensity{0.0f};
    float confidence{0.0f};
    float ambiguityMargin{0.0f};
    bool ambiguityDetected{false};
    bool fallbackRecommended{false};
    bool composable{false};
};

class NeuralIntentProtocol {
public:
    static constexpr const char* kSchemaV1 = "neuro.intent.embedding.v1";
    static constexpr const char* kSchemaV11 = "neuro.intent.embedding.v1.1";
    static constexpr const char* kLatestSchema = "neuro.intent.embedding.v1.1";

    NeuralIntentVector decode(const std::vector<float>& neuralVector, float baseConfidence) const;
    NeuralIntentVector decodeWithSchema(const std::vector<float>& neuralVector, float baseConfidence, const std::string& schemaVersion) const;
    bool isCompatibleSchema(const std::string& schemaVersion) const;
    std::string actionToken(NeuralIntentAction action) const;
};

} // namespace dnxr
