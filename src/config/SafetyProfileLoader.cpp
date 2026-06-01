#include "SafetyProfileLoader.h"

#include "../utils/Logger.h"

#include <algorithm>
#include <fstream>
#include <regex>
#include <sstream>

namespace dnxr {
namespace {

std::string readFile(const std::string& path) {
    std::ifstream input(path);
    if (!input) {
        return {};
    }
    std::ostringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
}

bool findString(const std::string& json, const std::string& key, std::string& out) {
    const std::regex pattern("\\\"" + key + "\\\"\\s*:\\s*\\\"([^\\\"]*)\\\"");
    std::smatch match;
    if (std::regex_search(json, match, pattern)) {
        out = match[1].str();
        return true;
    }
    return false;
}

bool findInt(const std::string& json, const std::string& key, int& out) {
    const std::regex pattern("\\\"" + key + "\\\"\\s*:\\s*(-?\\d+)");
    std::smatch match;
    if (!std::regex_search(json, match, pattern)) {
        return false;
    }
    try {
        out = std::stoi(match[1].str());
        return true;
    } catch (...) {
        return false;
    }
}

bool findFloat(const std::string& json, const std::string& key, float& out) {
    const std::regex pattern("\\\"" + key + "\\\"\\s*:\\s*([-+]?(?:\\d+\\.?\\d*|\\.\\d+)(?:[eE][-+]?\\d+)?)");
    std::smatch match;
    if (!std::regex_search(json, match, pattern)) {
        return false;
    }
    try {
        out = std::stof(match[1].str());
        return true;
    } catch (...) {
        return false;
    }
}

bool findBool(const std::string& json, const std::string& key, bool& out) {
    const std::regex pattern("\\\"" + key + "\\\"\\s*:\\s*(true|false)");
    std::smatch match;
    if (std::regex_search(json, match, pattern)) {
        out = match[1].str() == "true";
        return true;
    }
    return false;
}

float normalizeIntensity(float value) {
    return std::clamp(value, 0.0f, 1.0f);
}

} // namespace

SafetyRuntimeProfile SafetyProfileLoader::loadFromFile(const std::string& path) {
    const std::string json = readFile(path);
    if (json.empty()) {
        Logger::warn("[SafetyProfileLoader] safety profile not found, using conservative defaults: " + path);
        return SafetyRuntimeProfile{};
    }

    SafetyRuntimeProfile profile = loadFromString(json);
    std::string reason;
    if (!validate(profile, &reason)) {
        Logger::warn("[SafetyProfileLoader] invalid safety profile, using conservative defaults: " + reason);
        return SafetyRuntimeProfile{};
    }
    Logger::info("[SafetyProfileLoader] Loaded safety profile: " + path + " profile=" + profile.profile);
    return profile;
}

SafetyRuntimeProfile SafetyProfileLoader::loadFromString(
    const std::string& json,
    const SafetyRuntimeProfile& base) {

    SafetyRuntimeProfile profile = base;
    findString(json, "profile", profile.profile);
    findInt(json, "config_version", profile.configVersion);
    findFloat(json, "max_induced_vector_norm", profile.maxInducedVectorNorm);
    findFloat(json, "max_visual_intensity", profile.maxVisualIntensity);
    findFloat(json, "max_audio_intensity", profile.maxAudioIntensity);
    findFloat(json, "max_haptic_intensity", profile.maxHapticIntensity);
    findFloat(json, "max_vestibular_intensity", profile.maxVestibularIntensity);
    findFloat(json, "max_cognitive_load", profile.maxCognitiveLoad);
    findFloat(json, "max_drift_score", profile.maxDriftScore);
    findInt(json, "max_session_minutes", profile.maxSessionMinutes);
    findBool(json, "physiology_enforcement", profile.physiologyEnforcement);
    findBool(json, "emergency_stop_enabled", profile.emergencyStopEnabled);

    profile.maxInducedVectorNorm = std::clamp(profile.maxInducedVectorNorm, 0.0f, 1.0f);
    profile.maxVisualIntensity = normalizeIntensity(profile.maxVisualIntensity);
    profile.maxAudioIntensity = normalizeIntensity(profile.maxAudioIntensity);
    profile.maxHapticIntensity = normalizeIntensity(profile.maxHapticIntensity);
    profile.maxVestibularIntensity = normalizeIntensity(profile.maxVestibularIntensity);
    profile.maxCognitiveLoad = normalizeIntensity(profile.maxCognitiveLoad);
    profile.maxDriftScore = normalizeIntensity(profile.maxDriftScore);
    profile.maxSessionMinutes = std::max(1, profile.maxSessionMinutes);
    return profile;
}

bool SafetyProfileLoader::validate(const SafetyRuntimeProfile& profile, std::string* reason) {
    auto fail = [&](const std::string& message) {
        if (reason) {
            *reason = message;
        }
        return false;
    };

    if (profile.configVersion <= 0) {
        return fail("config_version must be positive");
    }
    if (profile.profile.empty()) {
        return fail("profile must not be empty");
    }
    if (profile.maxInducedVectorNorm <= 0.0f || profile.maxInducedVectorNorm > 0.25f) {
        return fail("max_induced_vector_norm must be in (0.0, 0.25] for simulation builds");
    }
    if (profile.maxVestibularIntensity > profile.maxHapticIntensity) {
        return fail("vestibular intensity must not exceed haptic intensity");
    }
    if (!profile.emergencyStopEnabled) {
        return fail("emergency_stop_enabled must remain true");
    }
    return true;
}

} // namespace dnxr
