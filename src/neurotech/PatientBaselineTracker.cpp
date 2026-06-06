#include "PatientBaselineTracker.h"
#include <algorithm>
namespace dnxr { PatientBaseline PatientBaselineTracker::update(const std::vector<float>& f,float lr){ float avg=0; for(float v:f) avg+=v; if(!f.empty()) avg/=static_cast<float>(f.size()); auto blend=[&](float& x){x=x*(1-lr)+avg*lr;}; blend(b_.resting); blend(b_.attention); blend(b_.motor); blend(b_.sensoryResponse); b_.stress=std::clamp(avg,0.0f,1.0f); b_.fatigue=std::clamp(1.0f-avg,0.0f,1.0f); return b_; } PatientBaseline PatientBaselineTracker::baseline() const{return b_;} }
