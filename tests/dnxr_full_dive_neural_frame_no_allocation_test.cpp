#include "full_dive/FullDiveNeuralFrame.h"
#include <cassert>
int main(){ dnxr::FullDiveNeuralFrame f; dnxr::NeuroChannelCommand c; c.intensity=0.4f; c.safetyLimit=0.5f; f.visual.channels.push_back(c); int n=0; f.forEachChannel([&](const auto&){++n;}); assert(n==1); assert(f.visual.channels.capacity()==dnxr::MAX_VISUAL_CHANNELS); return 0; }
