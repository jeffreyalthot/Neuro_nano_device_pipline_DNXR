#pragma once
#include <string>
namespace dnxr { class EmergencyLatch { public: void latch(const std::string& r){ latched_=true; reason_=r; } void resetForBench(){ latched_=false; reason_.clear(); } bool latched() const { return latched_; } const std::string& reason() const { return reason_; } private: bool latched_=false; std::string reason_; }; }
