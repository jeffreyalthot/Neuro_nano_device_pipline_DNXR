#include "neural_chip/NeuralChipSessionState.h"

namespace dnxr {

void resetNeuralChipSessionState(NeuralChipSessionState& state, std::uint64_t sessionId) {
    state = NeuralChipSessionState{};
    state.sessionId = sessionId;
}

} // namespace dnxr
