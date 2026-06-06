#pragma once
#include <cstdint>
#include <string>
#include <vector>
namespace dnxr {
struct PatientSpecificCalibrationProfile { std::string patientPseudonym; std::string profileId; std::string deviceId; std::string hash; std::string signature; std::int64_t createdAtUnixSeconds=0; std::int64_t expiresAtUnixSeconds=0; };
struct CalibrationProfileDecision { bool valid=false; std::vector<std::string> blockers; };
class PatientSpecificCalibrationProfileVerifier { public: CalibrationProfileDecision verify(const PatientSpecificCalibrationProfile& profile, std::int64_t nowUnixSeconds) const; };
}
