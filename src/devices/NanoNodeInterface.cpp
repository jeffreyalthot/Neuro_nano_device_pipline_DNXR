#include "NanoNodeInterface.h"

#include "../utils/Logger.h"

namespace dnxr {

std::vector<float> NanoNodeInterface::readNeuralNodeSignals() const {
    // This function returns a deterministic neural sample window for conceptual pipeline testing.
    return {0.1F, 0.3F, 0.8F, 0.2F};
}

void NanoNodeInterface::writeNeuralStimulus(const std::vector<float>& vec) const {
    // This function logs the outgoing stimulation payload size before hardware delivery.
    Logger::info("NanoNode stimulus size=" + std::to_string(vec.size()));
}

} // namespace dnxr
