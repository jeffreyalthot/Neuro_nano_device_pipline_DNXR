#pragma once
#include <map>
#include <string>
#include <vector>
namespace dnxr { struct FullDiveFrameScheduleDecision { bool runFrame=true; std::string reason="scheduled"; std::vector<std::string> stageOrder; }; class FullDiveFrameScheduler { public: explicit FullDiveFrameScheduler(float targetHz=90.0f); FullDiveFrameScheduleDecision plan(bool safetyReady, bool hardRealtimeReady) const; float targetHz() const; private: float targetHz_; }; }
