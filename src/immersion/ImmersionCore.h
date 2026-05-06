#pragma once
#include "SensoryRenderer.h"
#include <vector>
namespace dnxr { class ImmersionCore { public: std::vector<float> buildImmersiveFrame(const std::vector<float>& calibratedSignal); private: SensoryRenderer renderer_; VirtualWorldState world_; }; }
