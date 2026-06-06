#include "SeizureRiskGuard.h"
#include <cmath>
namespace dnxr {
SeizureRiskDecision SeizureRiskGuard::evaluate(const SensoryPatternSample& s) const { SeizureRiskDecision d; if(!std::isfinite(s.flashFrequencyHz)||!std::isfinite(s.contrastRatio)||!std::isfinite(s.transitionRateHz)) d.blockers.push_back("invalid_pattern"); if(s.flashFrequencyHz>=3.0&&s.flashFrequencyHz<=60.0) d.blockers.push_back("photosensitive_flash_frequency"); if(s.contrastRatio>0.80) d.blockers.push_back("extreme_contrast"); if(s.transitionRateHz>3.0) d.blockers.push_back("rapid_transition"); if(s.alternatingPattern&&s.contrastRatio>0.5) d.blockers.push_back("alternating_high_contrast_pattern"); d.allowed=d.blockers.empty(); return d; }
}
