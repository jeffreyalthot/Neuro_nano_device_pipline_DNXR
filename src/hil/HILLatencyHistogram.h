#pragma once
#include <vector>
namespace dnxr {
struct PercentileStats { double p50Ms=0.0; double p95Ms=0.0; double p99Ms=0.0; double maxMs=0.0; };
class HILLatencyHistogram { public: void record(double ms); PercentileStats stats() const; std::size_t count() const { return samples_.size(); } private: std::vector<double> samples_; };
}
