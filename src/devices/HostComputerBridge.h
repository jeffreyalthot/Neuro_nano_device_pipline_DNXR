#pragma once
#include <vector>
namespace dnxr { class HostComputerBridge { public: std::vector<float> uplink(const std::vector<float>& signal) const; std::vector<float> downlink(const std::vector<float>& command) const; }; }
