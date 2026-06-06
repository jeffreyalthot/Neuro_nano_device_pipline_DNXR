#pragma once
#include "SensoryFrameSchema.h"
#include <string>
#include <vector>
namespace dnxr { struct SceneCapabilityContract { std::vector<SensoryModality> requiredModalities,forbiddenModalities; float maxSensoryLoad=0.5f,vestibularRisk=0,seizureRisk=0,dissociationRisk=0,painRisk=0,emotionRisk=0; int allowedClinicalLevel=0; bool requiresRealityAnchor=true; bool acceptableForClinical(float budget, bool calibrated, bool realityAnchorAvailable) const; }; }
