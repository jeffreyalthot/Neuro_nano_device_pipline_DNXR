#include "ThermalPerceptionEngine.h"
#include <algorithm>
namespace dnxr { ThermalResult ThermalPerceptionEngine::update(const ThermalRequest& r,double){ if(r.warmth>0.35f||r.cold>0.35f||r.gradient>0.25f) return {0,0,adaptation_,true,"thermal_burn_risk_lockout"}; adaptation_=std::clamp(adaptation_+(r.warmth+r.cold)*0.005f,0.0f,0.2f); return {std::clamp(r.warmth-adaptation_,0.0f,0.2f),std::clamp(r.cold-adaptation_,0.0f,0.2f),adaptation_,false,"ok"}; } }
