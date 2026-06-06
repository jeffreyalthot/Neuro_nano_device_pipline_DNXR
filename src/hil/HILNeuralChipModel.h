#pragma once
#include "../hardware/NeuralChipAck.h"
#include "../hardware/NeuralChipHAL.h"
namespace dnxr { struct HILNeuralChipFaults { float variableLatencyMs=2,packetLossProbability=0,feedbackNoise=0,calibrationDrift=0,deviceTemperature=37; bool channelSaturation=false,hardwareCutoff=false,firmwareMismatch=false,unsupportedChannels=false,delayedAck=false; }; class HILNeuralChipModel { public: void setFaults(const HILNeuralChipFaults& faults); NeuralChipAck acknowledge(const SafeNeuralStimulusCommand& command) const; NeuralChipFrame feedback(std::uint64_t sequenceId) const; private: HILNeuralChipFaults faults_; }; }
