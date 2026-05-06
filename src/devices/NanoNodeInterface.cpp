#include "NanoNodeInterface.h"
#include "../utils/Logger.h"
namespace dnxr { std::vector<float> NanoNodeInterface::readNeuralNodeSignals() const { return {0.1F,0.3F,0.8F,0.2F}; } void NanoNodeInterface::writeNeuralStimulus(const std::vector<float>& vec) const { Logger::info("NanoNode stimulus size="+std::to_string(vec.size())); } }
