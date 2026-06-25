#pragma once
#include "SafetyKernelDecision.h"
#include "HardLimits.h"
#include "neural_chip/BoundedStimulusCommand.h"
namespace dnxr { class SafetyKernel { public: explicit SafetyKernel(HardLimits limits = {}) : limits_(limits) {} SafetyKernelDecision validate(const BoundedStimulusCommand& c) const { if(!c.validatedBySafetyKernel) return {false,"command_not_marked_safety_kernel_validated"}; if(c.normalizedCeiling>limits_.normalizedCeiling) return {false,"normalized_ceiling_exceeds_hard_limit"}; if(c.maxDeltaPerFrame>limits_.maxDeltaPerFrame) return {false,"delta_exceeds_hard_limit"}; if(c.cumulativeBudget>limits_.cumulativeBudget) return {false,"cumulative_budget_exceeds_hard_limit"}; return {true,"safety_kernel_allowed_bounded_command"}; } private: HardLimits limits_; }; }
