#pragma once
#include <string>
namespace dnxr { struct RealityAnchorHardMessage { std::string patientName,realDate,realTime,sessionTime,supervisionMessage,operatorVoice,realityReturnSignal; bool stopAvailable=true; }; class RealityAnchorHardChannel { public: RealityAnchorHardMessage compose(const std::string& patientName, const std::string& message) const; }; }
