#pragma once
#include "FiveSenseNeuralState.h"
#include <string>
namespace dnxr { struct FiveSenseFusionOutput { float globalPerceptualCoherence=1,visualAuditoryCoherence=1,visualVestibularCoherence=1,hapticProprioceptiveCoherence=1,smellTasteCoherence=1,bodySceneCoherence=1,realityAnchorStrength=1; bool safetyPriorityReduction=false; std::string reason="coherent"; }; class FiveSenseFusionModel { public: FiveSenseFusionOutput fuse(const FiveSenseNeuralState& state) const; }; }
