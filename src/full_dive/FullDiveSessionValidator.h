#pragma once
#include <string>
#include <vector>
namespace dnxr {
struct FullDiveSessionEvidence { bool emergencyExitAvailable=false; bool realityAnchorAvailable=false; bool decompressionAvailable=false; bool calibrationStable=false; bool temporalCoherenceMaintained=false; double sensoryExposureRatio=1.0; };
struct FullDiveSessionValidationResult { bool valid=false; std::vector<std::string> failures; };
class FullDiveSessionValidator { public: FullDiveSessionValidationResult validate(const FullDiveSessionEvidence& evidence) const; };
}
