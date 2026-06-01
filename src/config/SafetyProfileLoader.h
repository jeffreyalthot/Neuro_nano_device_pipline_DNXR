#pragma once

#include <string>

namespace dnxr {

struct SafetyRuntimeProfile {
    std::string profile = "simulation_safe";
    int configVersion = 1;
    float maxInducedVectorNorm = 0.12f;
    float maxVisualIntensity = 1.0f;
    float maxAudioIntensity = 0.85f;
    float maxHapticIntensity = 0.70f;
    float maxVestibularIntensity = 0.35f;
    float maxChemicalIntensity = 0.35f;
    float maxBodyIntensity = 0.60f;
    float maxDeltaPerFrame = 0.18f;
    double heartbeatTimeoutMs = 250.0;
    double latencyBudgetMs = 25.0;
    float maxCognitiveLoad = 0.75f;
    float maxDriftScore = 0.85f;
    int maxSessionMinutes = 30;
    bool physiologyEnforcement = true;
    bool emergencyStopEnabled = true;
};

class SafetyProfileLoader {
public:
    static SafetyRuntimeProfile loadFromFile(const std::string& path = "config/safety_config.json");
    static SafetyRuntimeProfile loadFromString(const std::string& json, const SafetyRuntimeProfile& base = SafetyRuntimeProfile{});
    static bool validate(const SafetyRuntimeProfile& profile, std::string* reason = nullptr);
};

} // namespace dnxr
