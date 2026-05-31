#include "../src/network/NeuralIntentProtocol.h"

#include <cassert>
#include <vector>

int main() {
    dnxr::NeuralIntentProtocol protocol;

    assert(protocol.isCompatibleSchema(dnxr::NeuralIntentProtocol::kSchemaV1));
    assert(protocol.isCompatibleSchema(dnxr::NeuralIntentProtocol::kSchemaV11));
    assert(!protocol.isCompatibleSchema("neuro.intent.embedding.v2"));

    const std::vector<float> composeVector{
        0.10f, 0.11f, 0.05f, 0.04f, 0.08f, 0.07f,
        0.02f, 0.01f, 0.03f, 0.02f, 0.92f, 0.88f
    };
    const dnxr::NeuralIntentVector compose = protocol.decodeWithSchema(
        composeVector, 0.86f, dnxr::NeuralIntentProtocol::kSchemaV11);
    assert(compose.schemaVersion == dnxr::NeuralIntentProtocol::kSchemaV11);
    assert(compose.action == dnxr::NeuralIntentAction::Compose);
    assert(compose.actionToken == "xr.compose");
    assert(compose.composable);
    assert(!compose.ambiguityDetected);
    assert(!compose.fallbackRecommended);

    const dnxr::NeuralIntentVector v1Compat = protocol.decodeWithSchema(
        composeVector, 0.86f, dnxr::NeuralIntentProtocol::kSchemaV1);
    assert(v1Compat.schemaVersion == dnxr::NeuralIntentProtocol::kSchemaV1);
    assert(v1Compat.action != dnxr::NeuralIntentAction::Compose);

    const std::vector<float> ambiguousVector{
        0.50f, 0.50f, 0.00f, 0.00f, 0.49f, 0.50f,
        0.01f, 0.01f, 0.02f, 0.01f, 0.00f, 0.00f
    };
    const dnxr::NeuralIntentVector ambiguous = protocol.decode(ambiguousVector, 0.90f);
    assert(ambiguous.action == dnxr::NeuralIntentAction::None);
    assert(ambiguous.actionToken == "none");
    assert(ambiguous.ambiguityDetected);
    assert(ambiguous.fallbackRecommended);
    assert(ambiguous.confidence <= 0.44f);

    const std::vector<float> moveVector{
        0.01f, 0.02f, 0.90f, 0.85f, 0.03f, 0.02f,
        0.01f, 0.01f, 0.02f, 0.01f, 0.02f, 0.01f
    };
    const dnxr::NeuralIntentVector move = protocol.decode(moveVector, 0.80f);
    assert(move.action == dnxr::NeuralIntentAction::Move);
    assert(move.actionToken == "xr.move");
    assert(move.composable);
    assert(move.confidence >= 0.0f && move.confidence <= 1.0f);

    return 0;
}
