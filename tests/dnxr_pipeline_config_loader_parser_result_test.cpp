#include "neural_chip/NeuralChipPipelineConfigLoader.h"
#include <cassert>
using namespace dnxr;
int main() {
    auto ok = NeuralChipPipelineConfigLoader::loadResultFromJsonText(R"({"strictConfig":true,"simulationOnly":true,"allowStimulusDispatch":false,"enableResearchVision":true,"researchModeAcknowledgement":true})");
    assert(ok.accepted);
    assert(ok.config.enableResearchVision);
    auto badType = NeuralChipPipelineConfigLoader::loadResultFromJsonText(R"({"strictConfig":true,"simulationOnly":"true"})");
    assert(!badType.accepted && !badType.errors.empty());
    auto badUnknown = NeuralChipPipelineConfigLoader::loadResultFromJsonText(R"({"strictConfig":true,"simulationOnly":true,"extra":false})");
    assert(!badUnknown.accepted);
    auto badResearch = NeuralChipPipelineConfigLoader::loadResultFromJsonText(R"({"simulationOnly":true,"allowStimulusDispatch":false,"enableResearchAuditory":true})");
    assert(!badResearch.accepted);
    auto missing = NeuralChipPipelineConfigLoader::loadResultFromFile("/tmp/dnxr_missing_061_config.json");
    assert(!missing.accepted && !missing.errors.empty());
    return 0;
}
