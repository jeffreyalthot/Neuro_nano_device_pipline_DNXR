#include "hardware/DeviceCapabilityLimiter.h"
#include <cassert>
int main(){ dnxr::FullDiveNeuralFrame f; dnxr::NeuroChannelCommand c; f.smellTaste.olfactory.channels.push_back(c); dnxr::ChipCapabilities caps; caps.supportsHardwareCutoff=true; caps.supportsClosedLoopFeedback=true; caps.canVisual=true; auto r=dnxr::DeviceCapabilityLimiter().apply(f,caps,90); assert(r.allowed); assert(r.frame.smellTaste.olfactory.channels.empty()); return 0; }
