#include "neural_chip/NeuralChipPipelineConfigValidator.h"

namespace dnxr {

NeuralChipPipelineConfigValidationResult NeuralChipPipelineConfigValidator::validate(const NeuralChipOnlyPipelineConfig& config,
                                                                                      const std::vector<std::string>& unknownKeys) {
    NeuralChipPipelineConfigValidationResult out{};
    auto reject = [&](const std::string& error) { out.accepted = false; out.errors.push_back(error); };
    if (!config.simulationOnly) reject("simulationOnly must be true");
    if (config.allowStimulusDispatch) reject("allowStimulusDispatch must be false");
    if (config.enableExperimentalChemicalChannels && !config.experimentalAcknowledgement) {
        reject("experimental chemical channels require experimentalAcknowledgement");
    }
    if ((config.enableResearchVision || config.enableResearchAuditory || config.enableResearchVestibular) && !config.researchModeAcknowledgement) {
        reject("research channels require researchModeAcknowledgement");
    }
    for (const auto& key : unknownKeys) {
        if (config.strictConfig) reject("unknown key in strict config: " + key);
        else out.warnings.push_back("unknown key: " + key);
    }
    return out;
}

} // namespace dnxr
