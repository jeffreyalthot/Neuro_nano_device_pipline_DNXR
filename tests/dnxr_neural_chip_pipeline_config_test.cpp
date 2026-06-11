#include "neural_chip/NeuralChipPipelineConfigLoader.h"
#include <cassert>

int main(){
    auto safe=dnxr::NeuralChipOnlyPipelineConfig::simulationSafeDefaults(); assert(!safe.enableExperimentalChemicalChannels); assert(!safe.capabilities.canVisual); assert(safe.capabilities.canHaptic);
    auto cfg=dnxr::NeuralChipPipelineConfigLoader::loadFromJsonText("{\"enableResearchVision\":true,\"enableResearchAuditory\":true,\"researchModeAcknowledgement\":true}"); assert(cfg.enableResearchVision); assert(cfg.capabilities.canVisual); assert(!cfg.enableExperimentalChemicalChannels);
}
