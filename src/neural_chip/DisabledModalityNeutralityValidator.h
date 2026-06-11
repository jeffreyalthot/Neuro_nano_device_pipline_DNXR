#pragma once
#include "neural_chip/ChipCapabilityContract.h"
namespace dnxr {
struct DisabledModalityNeutralityResult { bool accepted = false; const char* reason = ""; };
class DisabledModalityNeutralityValidator { public: DisabledModalityNeutralityResult validate(const FullDiveChipFrame& frame, const ChipCapabilities& capabilities) const; };
}
