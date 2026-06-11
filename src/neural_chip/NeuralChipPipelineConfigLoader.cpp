#include "neural_chip/NeuralChipPipelineConfigLoader.h"
#include "config/JsonParser.h"
#include "neural_chip/NeuralChipPipelineConfigValidator.h"

#include <fstream>
#include <set>
#include <sstream>

namespace dnxr {
namespace {
const std::set<std::string>& knownKeys() {
    static const std::set<std::string> keys = {
        "simulationOnly", "allowStimulusDispatch", "enableExperimentalChemicalChannels",
        "enableResearchVision", "enableResearchAuditory", "enableResearchVestibular",
        "experimentalAcknowledgement", "researchModeAcknowledgement", "strictConfig"
    };
    return keys;
}

bool readBool(const JsonValue& root, const std::string& key, bool fallback, PipelineConfigLoadResult& out, bool& seen) {
    const JsonValue* value = root.find(key);
    if (!value) { seen = false; return fallback; }
    seen = true;
    if (value->type() != JsonValue::Type::Bool) {
        out.errors.push_back("config key '" + key + "' must be boolean");
        return fallback;
    }
    return value->asBool(fallback);
}
} // namespace

PipelineConfigLoadResult NeuralChipPipelineConfigLoader::loadResultFromJsonText(const std::string& text) {
    PipelineConfigLoadResult out;
    out.config = NeuralChipOnlyPipelineConfig::simulationSafeDefaults();

    JsonValue root;
    std::string parseError;
    if (!JsonParser::tryParse(text, root, &parseError)) {
        out.errors.push_back("invalid JSON: " + parseError);
        return out;
    }
    if (!root.isObject()) {
        out.errors.push_back("pipeline config root must be a JSON object");
        return out;
    }

    bool seen = false;
    out.config.strictConfig = readBool(root, "strictConfig", false, out, seen);
    if (out.config.strictConfig) {
        for (const auto& kv : root.asObject()) {
            if (knownKeys().count(kv.first) == 0) out.errors.push_back("unknown strict config key: " + kv.first);
        }
    } else {
        for (const auto& kv : root.asObject()) {
            if (knownKeys().count(kv.first) == 0) out.warnings.push_back("ignored unknown config key: " + kv.first);
        }
    }

    out.config.simulationOnly = readBool(root, "simulationOnly", true, out, seen);
    out.config.allowStimulusDispatch = readBool(root, "allowStimulusDispatch", false, out, seen);
    out.config.enableExperimentalChemicalChannels = readBool(root, "enableExperimentalChemicalChannels", false, out, seen);
    out.config.enableResearchVision = readBool(root, "enableResearchVision", false, out, seen);
    out.config.enableResearchAuditory = readBool(root, "enableResearchAuditory", false, out, seen);
    out.config.enableResearchVestibular = readBool(root, "enableResearchVestibular", false, out, seen);
    out.config.experimentalAcknowledgement = readBool(root, "experimentalAcknowledgement", false, out, seen);
    out.config.researchModeAcknowledgement = readBool(root, "researchModeAcknowledgement", false, out, seen);

    if (!out.config.simulationOnly) out.errors.push_back("simulationOnly must be true");
    if (out.config.allowStimulusDispatch) out.errors.push_back("allowStimulusDispatch must be false");
    if (out.config.enableExperimentalChemicalChannels && !out.config.experimentalAcknowledgement) {
        out.errors.push_back("chemical mode requires experimentalAcknowledgement");
    }
    if ((out.config.enableResearchVision || out.config.enableResearchAuditory || out.config.enableResearchVestibular) &&
        !out.config.researchModeAcknowledgement) {
        out.errors.push_back("research mode requires researchModeAcknowledgement");
    }

    out.config.capabilities.canVisual = out.config.enableResearchVision;
    out.config.capabilities.canAuditory = out.config.enableResearchAuditory;
    out.config.capabilities.canVestibular = out.config.enableResearchVestibular;
    out.config.capabilities.canOlfactory = out.config.enableExperimentalChemicalChannels;
    out.config.capabilities.canGustatory = out.config.enableExperimentalChemicalChannels;
    out.config.patientMap.visual.enabled = out.config.enableResearchVision;
    out.config.patientMap.auditory.enabled = out.config.enableResearchAuditory;
    out.config.patientMap.vestibular.enabled = out.config.enableResearchVestibular;
    out.config.patientMap.olfactory.enabled = out.config.enableExperimentalChemicalChannels;
    out.config.patientMap.gustatory.enabled = out.config.enableExperimentalChemicalChannels;

    const auto validation = NeuralChipPipelineConfigValidator::validate(out.config);
    if (!validation.accepted) out.errors.insert(out.errors.end(), validation.errors.begin(), validation.errors.end());
    out.warnings.insert(out.warnings.end(), validation.warnings.begin(), validation.warnings.end());
    out.accepted = out.errors.empty();
    if (!out.accepted) out.config = NeuralChipOnlyPipelineConfig::simulationSafeDefaults();
    return out;
}

PipelineConfigLoadResult NeuralChipPipelineConfigLoader::loadResultFromFile(const std::string& path) {
    PipelineConfigLoadResult out;
    out.config = NeuralChipOnlyPipelineConfig::simulationSafeDefaults();
    std::ifstream input(path);
    if (!input) {
        out.errors.push_back("config file not found: " + path);
        return out;
    }
    std::ostringstream buffer;
    buffer << input.rdbuf();
    return loadResultFromJsonText(buffer.str());
}

NeuralChipOnlyPipelineConfig NeuralChipPipelineConfigLoader::loadFromJsonText(const std::string& text) {
    const auto result = loadResultFromJsonText(text);
    return result.accepted ? result.config : NeuralChipOnlyPipelineConfig::simulationSafeDefaults();
}

NeuralChipOnlyPipelineConfig NeuralChipPipelineConfigLoader::loadFromFile(const std::string& path) {
    const auto result = loadResultFromFile(path);
    return result.accepted ? result.config : NeuralChipOnlyPipelineConfig::simulationSafeDefaults();
}

} // namespace dnxr
