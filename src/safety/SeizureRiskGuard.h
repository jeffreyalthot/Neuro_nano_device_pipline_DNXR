#pragma once
#include <string>
#include <vector>
namespace dnxr {
struct SensoryPatternSample { double flashFrequencyHz=0.0; double contrastRatio=0.0; double transitionRateHz=0.0; bool alternatingPattern=false; };
struct SeizureRiskDecision { bool allowed=false; std::vector<std::string> blockers; };
class SeizureRiskGuard { public: SeizureRiskDecision evaluate(const SensoryPatternSample& sample) const; };
}
