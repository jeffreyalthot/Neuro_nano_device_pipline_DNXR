#include "../src/config/SafetyProfileLoader.h"

#include <cassert>
#include <iostream>
#include <string>

int main() {
    const std::string json = R"json({
      "config_version": 7,
      "profile": "unit_test_profile",
      "max_induced_vector_norm": 0.10,
      "max_visual_intensity": 0.9,
      "max_audio_intensity": 0.8,
      "max_haptic_intensity": 0.5,
      "max_vestibular_intensity": 0.2,
      "max_session_minutes": 12,
      "max_cognitive_load": 0.6,
      "max_drift_score": 0.7,
      "physiology_enforcement": true,
      "emergency_stop_enabled": true
    })json";

    const dnxr::SafetyRuntimeProfile profile = dnxr::SafetyProfileLoader::loadFromString(json);
    assert(profile.profile == "unit_test_profile");
    assert(profile.configVersion == 7);
    assert(profile.maxInducedVectorNorm == 0.10f);
    assert(profile.maxVestibularIntensity <= profile.maxHapticIntensity);

    std::string reason;
    assert(dnxr::SafetyProfileLoader::validate(profile, &reason));

    auto invalid = profile;
    invalid.maxInducedVectorNorm = 0.5f;
    assert(!dnxr::SafetyProfileLoader::validate(invalid, &reason));

    std::cout << "Safety profile loader validation OK\n";
    return 0;
}
