#include "neural_chip/NeuralChipPipelineConfigLoader.h"

#include <fstream>
#include <sstream>

namespace dnxr {
namespace {
bool jsonBool(const std::string& text, const char* key, bool fallback) {
    const std::string needle = std::string("\"") + key + "\"";
    const auto keyPos = text.find(needle);
    if (keyPos == std::string::npos) return fallback;
    const auto colon = text.find(':', keyPos);
    if (colon == std::string::npos) return fallback;
    const auto truePos = text.find("true", colon);
    const auto falsePos = text.find("false", colon);
    if (truePos != std::string::npos && (falsePos == std::string::npos || truePos < falsePos)) return true;
    if (falsePos != std::string::npos) return false;
    return fallback;
}
} // namespace

NeuralChipOnlyPipelineConfig NeuralChipPipelineConfigLoader::loadFromJsonText(const std::string& text) {
    NeuralChipOnlyPipelineConfig config = NeuralChipOnlyPipelineConfig::simulationSafeDefaults();
    config.enableExperimentalChemicalChannels = jsonBool(text, "enableExperimentalChemicalChannels", false);
    config.enableResearchVision = jsonBool(text, "enableResearchVision", false);
    config.enableResearchAuditory = jsonBool(text, "enableResearchAuditory", false);
    config.enableResearchVestibular = jsonBool(text, "enableResearchVestibular", false);
    config.capabilities.canVisual = config.enableResearchVision;
    config.capabilities.canAuditory = config.enableResearchAuditory;
    config.capabilities.canVestibular = config.enableResearchVestibular;
    config.capabilities.canOlfactory = config.enableExperimentalChemicalChannels;
    config.capabilities.canGustatory = config.enableExperimentalChemicalChannels;
    config.patientMap.visual.enabled = config.enableResearchVision;
    config.patientMap.auditory.enabled = config.enableResearchAuditory;
    config.patientMap.vestibular.enabled = config.enableResearchVestibular;
    config.patientMap.olfactory.enabled = config.enableExperimentalChemicalChannels;
    config.patientMap.gustatory.enabled = config.enableExperimentalChemicalChannels;
    return config;
}

NeuralChipOnlyPipelineConfig NeuralChipPipelineConfigLoader::loadFromFile(const std::string& path) {
    std::ifstream input(path);
    std::ostringstream buffer;
    buffer << input.rdbuf();
    return loadFromJsonText(buffer.str());
}

} // namespace dnxr
