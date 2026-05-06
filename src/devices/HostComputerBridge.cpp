#include "HostComputerBridge.h"
namespace dnxr { std::vector<float> HostComputerBridge::uplink(const std::vector<float>& signal) const { return signal; } std::vector<float> HostComputerBridge::downlink(const std::vector<float>& command) const { return command; } }
