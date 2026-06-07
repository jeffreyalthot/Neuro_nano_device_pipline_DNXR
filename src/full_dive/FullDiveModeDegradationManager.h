#pragma once
#include "../hardware/ChipCapabilityNegotiation.h"
#include "../immersion/SensoryFrameSchema.h"
#include <array>
#include <string>
namespace dnxr {
enum class FullDiveRuntimeMode { NoHumanMode, SimulationOnly, VisualAudioOnly, XRHapticNonInvasive, PartialNeuralClosedLoop, FullFiveSenseClosedLoop, EmergencyDecompressionOnly };
struct FullDiveModeDecision { FullDiveRuntimeMode mode=FullDiveRuntimeMode::NoHumanMode; bool fullDiveClaimAllowed=false; std::string auditReason="not_evaluated"; };
struct FullDiveModalityReadiness { std::array<bool, static_cast<std::size_t>(SensoryModality::Count)> supported{}; std::array<bool, static_cast<std::size_t>(SensoryModality::Count)> calibrated{}; std::array<bool, static_cast<std::size_t>(SensoryModality::Count)> feedback{}; bool simulationOnly=true; bool emergency=false; };
class FullDiveModeDegradationManager { public: FullDiveModeDecision evaluate(const FullDiveModalityReadiness& readiness) const; };
}
