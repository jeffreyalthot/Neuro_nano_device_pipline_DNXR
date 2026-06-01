#include "SafetyProfileLoader.h"
#include "JsonParser.h"

#include "../utils/Logger.h"

#include <algorithm>
#include <fstream>
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
    JsonValue root;
    std::string error;
    if (!JsonParser::tryParse(json, root, &error) || !root.isObject()) {
        Logger::warn("[SafetyProfileLoader] invalid safety profile JSON, using base profile: " + error);
        return profile;
    }

    profile.profile = root.getString("profile", profile.profile);
    profile.configVersion = root.getInt("config_version", profile.configVersion);
    profile.maxInducedVectorNorm = root.getFloat("max_induced_vector_norm", profile.maxInducedVectorNorm);
    profile.maxVisualIntensity = root.getFloat("max_visual_intensity", profile.maxVisualIntensity);
    profile.maxAudioIntensity = root.getFloat("max_audio_intensity", profile.maxAudioIntensity);
    profile.maxHapticIntensity = root.getFloat("max_haptic_intensity", profile.maxHapticIntensity);
    profile.maxVestibularIntensity = root.getFloat("max_vestibular_intensity", profile.maxVestibularIntensity);
    profile.maxChemicalIntensity = root.getFloat("max_chemical_intensity", profile.maxChemicalIntensity);
    profile.maxBodyIntensity = root.getFloat("max_body_intensity", profile.maxBodyIntensity);
    profile.maxProprioceptiveIntensity = root.getFloat(
        "max_proprioceptive_intensity",
        root.getFloat("max_body_intensity", profile.maxProprioceptiveIntensity));
    profile.maxDeltaPerFrame = root.getFloat("max_delta_per_frame", profile.maxDeltaPerFrame);
    profile.heartbeatTimeoutMs = root.getDouble("heartbeat_timeout_ms", profile.heartbeatTimeoutMs);
    profile.latencyBudgetMs = root.getDouble("latency_budget_ms", profile.latencyBudgetMs);
    profile.maxCognitiveLoad = root.getFloat("max_cognitive_load", profile.maxCognitiveLoad);
    profile.maxDriftScore = root.getFloat("max_drift_score", profile.maxDriftScore);
    profile.maxSessionMinutes = root.getInt("max_session_minutes", profile.maxSessionMinutes);
    profile.physiologyEnforcement = root.getBool("physiology_enforcement", profile.physiologyEnforcement);
    profile.emergencyStopEnabled = root.getBool("emergency_stop_enabled", profile.emergencyStopEnabled);
    profile.minFullDiveStability = root.getFloat("min_full_dive_stability", profile.minFullDiveStability);
    profile.minTemporalCoherence = root.getFloat("min_temporal_coherence", profile.minTemporalCoherence);
    profile.minVestibularStability = root.getFloat("min_vestibular_stability", profile.minVestibularStability);
    profile.minMotorPredictionAccuracy = root.getFloat("min_motor_prediction_accuracy", profile.minMotorPredictionAccuracy);

    profile.maxInducedVectorNorm = std::clamp(profile.maxInducedVectorNorm, 0.0f, 1.0f);
    profile.maxVisualIntensity = normalizeIntensity(profile.maxVisualIntensity);
    profile.maxAudioIntensity = normalizeIntensity(profile.maxAudioIntensity);
    profile.maxHapticIntensity = normalizeIntensity(profile.maxHapticIntensity);
    profile.maxVestibularIntensity = normalizeIntensity(profile.maxVestibularIntensity);
    profile.maxChemicalIntensity = normalizeIntensity(profile.maxChemicalIntensity);
    profile.maxBodyIntensity = normalizeIntensity(profile.maxBodyIntensity);
    profile.maxProprioceptiveIntensity = normalizeIntensity(profile.maxProprioceptiveIntensity);
    profile.maxDeltaPerFrame = normalizeIntensity(profile.maxDeltaPerFrame);
    profile.heartbeatTimeoutMs = std::max(1.0, profile.heartbeatTimeoutMs);
    profile.latencyBudgetMs = std::max(1.0, profile.latencyBudgetMs);
    profile.maxCognitiveLoad = normalizeIntensity(profile.maxCognitiveLoad);
    profile.maxDriftScore = normalizeIntensity(profile.maxDriftScore);
    profile.maxSessionMinutes = std::max(1, profile.maxSessionMinutes);
    profile.minFullDiveStability = normalizeIntensity(profile.minFullDiveStability);
    profile.minTemporalCoherence = normalizeIntensity(profile.minTemporalCoherence);
    profile.minVestibularStability = normalizeIntensity(profile.minVestibularStability);
    profile.minMotorPredictionAccuracy = normalizeIntensity(profile.minMotorPredictionAccuracy);
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
    if (profile.maxProprioceptiveIntensity > profile.maxHapticIntensity) {
        return fail("proprioceptive intensity must not exceed haptic intensity");
    }
    if (profile.maxVestibularIntensity > profile.maxProprioceptiveIntensity) {
        return fail("vestibular intensity must not exceed proprioceptive intensity");
    }
    if (profile.maxChemicalIntensity > profile.maxHapticIntensity) {
        return fail("chemical intensity must not exceed haptic intensity");
    }
    if (profile.maxDeltaPerFrame <= 0.0f || profile.maxDeltaPerFrame > 1.0f) {
        return fail("max_delta_per_frame must be in (0.0, 1.0]");
    }
    if (profile.heartbeatTimeoutMs < 10.0 || profile.latencyBudgetMs < 1.0) {
        return fail("watchdog timing budgets are below simulation minimums");
    }
    if (!profile.emergencyStopEnabled) {
        return fail("emergency_stop_enabled must remain true");
    }
    if (profile.minFullDiveStability <= 0.0f || profile.minTemporalCoherence <= 0.0f ||
        profile.minVestibularStability <= 0.0f || profile.minMotorPredictionAccuracy <= 0.0f) {
        return fail("full-dive safety thresholds must be positive");
    }
    return true;
}

} // namespace dnxr
