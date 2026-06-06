#pragma once
#include <string>
namespace dnxr { struct SceneTransitionDelta { float body=0,light=0,gravity=0,scale=0,audio=0; bool teleport=false; }; struct SceneTransitionResult { bool allowed=true; std::string reason="smooth_transition"; }; class SceneTransitionSafetyEngine { public: SceneTransitionResult evaluate(const SceneTransitionDelta& d) const; }; }
