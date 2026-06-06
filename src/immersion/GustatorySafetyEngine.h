#pragma once
#include <string>
namespace dnxr { struct GustatoryRequest { float intensity=0, frequencyHz=0, durationMs=0; bool contextValid=false; }; struct GustatorySafetyResult { bool allowed=true, forceNeutralization=false; float boundedIntensity=0; std::string reason="ok"; }; class GustatorySafetyEngine { public: GustatorySafetyResult evaluate(const GustatoryRequest& r) const; }; }
