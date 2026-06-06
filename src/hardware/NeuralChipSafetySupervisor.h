#pragma once
#include "NeuralChipAck.h"
#include <string>
namespace dnxr { enum class NeuralChipSafetyAction { Allow,Reduce,HoldLastSafeFrame,RealityAnchor,Decompress,EmergencyStop }; struct NeuralChipSafetyDecision { NeuralChipSafetyAction action=NeuralChipSafetyAction::EmergencyStop; std::string reason="not_evaluated"; bool allowed=false; }; class NeuralChipSafetySupervisor { public: NeuralChipSafetyDecision evaluate(const NeuralChipAck& ack, bool firmwareMatch, int lostPackets) const; }; }
