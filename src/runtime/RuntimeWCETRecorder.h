#pragma once
namespace dnxr { class RuntimeWCETRecorder { public: void observe(double ms); double worstCaseMs() const; bool within(double budgetMs) const; private: double worst_=0.0; }; }
