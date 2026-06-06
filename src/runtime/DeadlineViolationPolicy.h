#pragma once
#include <string>
namespace dnxr {
struct DeadlineObservation { std::string stage; double budgetMs=0.0; double observedMs=0.0; };
struct DeadlinePolicyDecision { bool safeStateRequired=false; std::string reason; };
class DeadlineViolationPolicy { public: DeadlinePolicyDecision evaluate(const DeadlineObservation& observation) const; };
}
