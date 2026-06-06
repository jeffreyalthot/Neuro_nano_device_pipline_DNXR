#pragma once
#include <string>
namespace dnxr { struct FlashPatternSignals { bool strobe=false,rapidContrast=false,highFrequencyFlicker=false,redBlueAlternation=false,highLuminanceBursts=false,patternRepetition=false; }; struct FlashPatternBlockerResult { bool safe=true; std::string action="Allow"; std::string reason="ok"; }; class FlashPatternBlocker { public: FlashPatternBlockerResult evaluate(const FlashPatternSignals& i) const; }; }
