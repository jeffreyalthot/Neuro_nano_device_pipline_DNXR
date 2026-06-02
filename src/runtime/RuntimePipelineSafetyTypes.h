#pragma once

#include "FullDiveRuntimeSnapshot.h"
#include "../hardware/NeuralChipHAL.h"
#include "../safety/RealityAnchorSystem.h"
#include "../safety/SafetyGovernor.h"

namespace dnxr {

// Safety and intervention decisions produced by the runtime pipeline.
using RuntimePipelineSafetyDecision = SafetyDecision;
using RuntimePipelineRealityAnchorPlan = RealityAnchorPlan;
using RuntimePipelineFullDiveSnapshot = FullDiveRuntimeSnapshot;
using RuntimePipelineStimulusCommand = SafeNeuralStimulusCommand;

} // namespace dnxr
