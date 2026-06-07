#pragma once
#include "SensoryFrameSchema.h"
#include <string>
#include <vector>
namespace dnxr { struct SceneCapabilityContract { std::string sceneId; std::string sceneHash; std::string signature; std::vector<SensoryModality> requiredModalities,forbiddenModalities; std::vector<std::string> requiredCalibrations; float maxSensoryLoad=0.5f,vestibularRisk=0,seizureRisk=0,dissociationRisk=0,painRisk=0,emotionRisk=0; float maxVisualMotion=0.25f,maxAudioStartle=0.1f,maxHapticImpact=0.1f,maxThermalGradient=0.0f,maxVestibularLoad=0.1f,maxOlfactoryPersistence=0.1f,maxGustatoryDose=0.1f; int allowedClinicalLevel=0; bool requiresRealityAnchor=true; bool requiresNeutralExit=true; bool acceptableForClinical(float budget, bool calibrated, bool realityAnchorAvailable) const; }; }
