#include "GustatorySafetyEngine.h"
#include <algorithm>
namespace dnxr { GustatorySafetyResult GustatorySafetyEngine::evaluate(const GustatoryRequest& r) const { if(!r.contextValid) return {false,true,0,"gustatory_context_required"}; if(r.frequencyHz>0.2f) return {false,true,0,"continuous_taste_blocked"}; if(r.durationMs>5000) return {false,true,0,"gustatory_duration_limit"}; return {true,false,std::clamp(r.intensity,0.0f,0.08f),"ok"}; } }
