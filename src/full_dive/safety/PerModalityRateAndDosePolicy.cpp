#include "full_dive/safety/PerModalityRateAndDosePolicy.h"

namespace dnxr {

PerModalityRateAndDosePolicy::PerModalityRateAndDosePolicy(FullDiveDosePolicy policy) : policy_(policy) {}
const FullDiveDosePolicy& PerModalityRateAndDosePolicy::policy() const { return policy_; }
void PerModalityRateAndDosePolicy::setPolicy(FullDiveDosePolicy policy) { policy_ = policy; }

} // namespace dnxr
