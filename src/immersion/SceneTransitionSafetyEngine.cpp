#include "SceneTransitionSafetyEngine.h"
#include <algorithm>
#include <cmath>
namespace dnxr { SceneTransitionResult SceneTransitionSafetyEngine::evaluate(const SceneTransitionDelta& d) const { if(d.teleport) return {false,"abrupt_teleportation_blocked"}; if(std::max({std::fabs(d.body),std::fabs(d.light),std::fabs(d.gravity),std::fabs(d.scale),std::fabs(d.audio)})>0.30f) return {false,"abrupt_scene_transition_blocked"}; return {}; } }
