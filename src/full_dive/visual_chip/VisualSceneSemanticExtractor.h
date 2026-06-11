#pragma once
#include "full_dive/visual_chip/VisualCorticalWorldEncoder.h"
namespace dnxr { struct VisualSceneSemantics { float nearObstacles01=0.0f; float walkingCorridor01=0.0f; float handsVisible01=0.0f; float avatarBodyVisible01=0.0f; float interactiveObjects01=0.0f; float gazeDirectionConfidence01=0.0f; float horizon01=0.0f; float globalMotion01=0.0f; float visualDanger01=0.0f; float importantText01=0.0f; }; class VisualSceneSemanticExtractor { public: VisualSceneSemantics extract(const VirtualSceneFrame& scene) const; private: static float clamp01(float value); }; }
