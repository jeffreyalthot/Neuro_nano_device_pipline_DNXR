#pragma once
#include "hardware/NeuralChipHAL.h"
#include "HardwareBenchSession.h"
namespace dnxr { struct HardwareBenchSelfTestResult { bool deviceReachable=false; bool firmwareVersionReadable=false; bool capabilitiesReadable=false; bool ackChannelAlive=false; bool emergencyStopAccepted=false; bool telemetryChannelAlive=false; bool outputDisabledByDefault=true; bool passed() const { return deviceReachable&&firmwareVersionReadable&&capabilitiesReadable&&ackChannelAlive&&emergencyStopAccepted&&telemetryChannelAlive&&outputDisabledByDefault; } }; class HardwareBenchSelfTest { public: HardwareBenchSelfTestResult runDryRunSelfTest(RealNeuralChipHAL& hal, const HardwareBenchSessionManifest& manifest) const; }; }
