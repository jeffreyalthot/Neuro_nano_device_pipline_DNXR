#include "ImmersionCore.h"
namespace dnxr { std::vector<float> ImmersionCore::buildImmersiveFrame(const std::vector<float>& calibratedSignal){ if(!calibratedSignal.empty()) world_.intensity=calibratedSignal[0]; return renderer_.renderSensoryFrame(world_);} }
