#pragma once
#include <string>
namespace dnxr { struct VestibularMotionRequest { float angularVelocity=0, linearAcceleration=0, jerk=0, tiltIllusion=0, fallIllusion=0; }; struct VestibularHardLimitResult { bool allowed=true; bool cooldownRequired=false; std::string reason="ok"; }; class VestibularHardLimiter { public: VestibularHardLimitResult evaluate(const VestibularMotionRequest& r) const; float maxAngularVelocity=0.35f, maxLinearAcceleration=0.25f, maxJerk=0.20f, maxTiltIllusion=0.15f, maxFallIllusion=0.05f; }; }
