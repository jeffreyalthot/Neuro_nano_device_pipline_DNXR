#pragma once
#include "full_dive/touch_chip/NeuralVirtualEpidermisEncoder.h"
#include <vector>
namespace dnxr { struct SomatotopicContactCell { BodyRegion region=BodyRegion::Other; float pressure01=0.0f; float area01=0.0f; float slip01=0.0f; float texture01=0.0f; float temperature01=0.0f; float confidence01=0.0f; }; struct AvatarSomatotopicSurfaceFrame { std::vector<SomatotopicContactCell> cells; float totalLoad01=0.0f; float asymmetry01=0.0f; float overloadRisk01=0.0f; }; class AvatarSomatotopicSurfaceMap { public: AvatarSomatotopicSurfaceFrame map(const std::vector<VirtualContactPatch>& patches) const; NeuralTouchVector toTouchVector(const AvatarSomatotopicSurfaceFrame& frame) const; private: static float clamp01(float value); }; }
