#pragma once
#include <vector>
namespace dnxr { struct LatencyReport { double averageMs=0.0; double worstMs=0.0; bool withinBudget=false; }; class LatencyInstrumentation { public: void record(double latencyMs); LatencyReport report(double budgetMs) const; private: std::vector<double> samples_; }; }
