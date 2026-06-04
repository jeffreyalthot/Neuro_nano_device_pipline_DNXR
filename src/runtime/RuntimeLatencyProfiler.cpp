#include "RuntimeLatencyProfiler.h"
#include <algorithm>
namespace dnxr { void RuntimeLatencyProfiler::record(double ms){samples_.push_back(ms);} RuntimeLatencyReport RuntimeLatencyProfiler::report() const{ RuntimeLatencyReport r; if(samples_.empty()) return r; auto s=samples_; std::sort(s.begin(),s.end()); double sum=0; for(double v:s){sum+=v;if(v>r.worst)r.worst=v;} r.average=sum/static_cast<double>(s.size()); auto pct=[&](double q){auto idx=static_cast<std::size_t>((s.size()-1)*q); return s[idx];}; r.p95=pct(0.95); r.p99=pct(0.99); return r;} }
