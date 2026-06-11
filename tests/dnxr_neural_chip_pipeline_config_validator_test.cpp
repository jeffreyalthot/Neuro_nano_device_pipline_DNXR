#include "neural_chip/NeuralChipPipelineConfigValidator.h"
#include <cassert>
int main(){ auto c=dnxr::NeuralChipOnlyPipelineConfig::simulationSafeDefaults(); assert(dnxr::NeuralChipPipelineConfigValidator::validate(c).accepted); c.enableResearchVision=true; assert(!dnxr::NeuralChipPipelineConfigValidator::validate(c).accepted); c.researchModeAcknowledgement=true; assert(dnxr::NeuralChipPipelineConfigValidator::validate(c).accepted); c.enableExperimentalChemicalChannels=true; assert(!dnxr::NeuralChipPipelineConfigValidator::validate(c).accepted); }
