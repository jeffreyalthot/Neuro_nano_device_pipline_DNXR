#pragma once

#include "neural_chip/NeuralChipFrame.h"

namespace dnxr {

struct CandidateChipFrame { FullDiveChipFrame frame; };
struct HoldChipFrame { FullDiveChipFrame frame; const char* holdKind = "hold"; };
struct CommittedChipFrame { FullDiveChipFrame frame; };

} // namespace dnxr
