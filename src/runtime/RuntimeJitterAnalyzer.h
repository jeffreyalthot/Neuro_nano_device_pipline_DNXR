#pragma once
#include <vector>
namespace dnxr { class RuntimeJitterAnalyzer { public: void recordPeriod(double ms); double maxJitterMs() const; bool within(double budgetMs) const; private: std::vector<double> periods_; }; }
