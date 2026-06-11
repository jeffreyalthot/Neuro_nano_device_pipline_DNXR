#pragma once
#include "neural_chip/NeuralChipFrame.h"
namespace dnxr {
struct VirtualTasteFrame { float sweet01=0.0f, salty01=0.0f, sour01=0.0f, bitter01=0.0f, umami01=0.0f, metallic01=0.0f, mouthfeel01=0.0f, temperature01=0.0f, pleasantness01=0.0f; };
struct PatientGustatoryMap { bool experimentalChannelEnabled=false; float discreteDoseBudget01=0.20f; };
struct GustatorySafetyState { bool allowSwallowingInterference=false; float nauseaRisk01=0.0f; float swallowingConflictRisk01=0.0f; };
class NeuralGustatoryEncoder { public: NeuralGustatoryVector encode(const VirtualTasteFrame& taste,const PatientGustatoryMap& map,const GustatorySafetyState& safety) const; };
}
