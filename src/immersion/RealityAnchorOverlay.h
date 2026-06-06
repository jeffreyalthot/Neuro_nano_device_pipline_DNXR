#pragma once
#include <string>
namespace dnxr { struct RealityAnchorOverlayState { std::string patientName,dateReal,operatorVoiceMessage="vous êtes dans un test supervisé"; double sessionTimeSeconds=0; float intensityLevel=0; bool stopButtonVisible=true,exitAvailable=true; }; class RealityAnchorOverlay { public: std::string renderText(const RealityAnchorOverlayState& s) const; }; }
