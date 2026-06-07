#pragma once
#include "../full_dive/FullDiveNeuralFrame.h"
#include <array>
#include <string>
namespace dnxr {
struct ExposureLimitSet { std::array<float,10> perFrame{}, perMinute{}, perSession{}; };
struct FiveSenseExposureReport { bool withinLimits=true; bool cooldownRequired=false; std::string reason="within_limits"; std::array<float,10> sessionDose{}; };
class FiveSenseExposureLedger { public: FiveSenseExposureLedger(); FiveSenseExposureReport recordFrame(const FullDiveNeuralFrame& frame, float dtSeconds); void setLimits(const ExposureLimitSet& limits); void resetSession(); private: ExposureLimitSet limits_{}; std::array<float,10> sessionDose_{}; std::array<float,10> minuteDose_{}; float minuteWindowSeconds_=0.0f; };
}
