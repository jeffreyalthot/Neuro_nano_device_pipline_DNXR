#include "DeadlineViolationPolicy.h"
namespace dnxr { DeadlinePolicyDecision DeadlineViolationPolicy::evaluate(const DeadlineObservation& o) const { DeadlinePolicyDecision d; d.safeStateRequired = o.budgetMs <= 0.0 || o.observedMs > o.budgetMs; d.reason = d.safeStateRequired ? "deadline_violation:" + o.stage : "within_budget"; return d; } }
