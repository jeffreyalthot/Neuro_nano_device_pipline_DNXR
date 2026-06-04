#pragma once

#include <vector>

namespace dnxr {

struct RuntimeLatencyReport {
    double average = 0.0;
    double p95 = 0.0;
    double p99 = 0.0;
    double worst = 0.0;
};

class RuntimeLatencyProfiler {
public:
    void record(double ms);
    RuntimeLatencyReport report() const;

private:
    std::vector<double> samples_;
};

} // namespace dnxr
