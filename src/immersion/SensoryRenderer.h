#pragma once
#include "VirtualWorldState.h"
#include <vector>
namespace dnxr { class SensoryRenderer { public: std::vector<float> renderSensoryFrame(const VirtualWorldState& state) const; }; }
