#include "hil/HILLatencyHistogram.h"
#include <algorithm>
#include <cmath>
namespace dnxr { void HILLatencyHistogram::record(double ms){ samples_.push_back(ms < 0.0 ? 0.0 : ms); } PercentileStats HILLatencyHistogram::stats() const { PercentileStats s; if(samples_.empty()) return s; auto v=samples_; std::sort(v.begin(),v.end()); auto pick=[&](double p){ auto idx=static_cast<std::size_t>(std::ceil((p/100.0)*v.size())); if(idx==0) idx=1; if(idx>v.size()) idx=v.size(); return v[idx-1]; }; s.p50Ms=pick(50); s.p95Ms=pick(95); s.p99Ms=pick(99); s.maxMs=v.back(); return s; } }
