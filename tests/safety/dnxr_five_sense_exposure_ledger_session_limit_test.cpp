#include "../../src/safety/FiveSenseExposureLedger.h"
#include <cassert>
int main(){ dnxr::FiveSenseExposureLedger l; dnxr::ExposureLimitSet lim; lim.perFrame.fill(10); lim.perMinute.fill(10); lim.perSession.fill(0.01f); l.setLimits(lim); dnxr::FullDiveNeuralFrame f; dnxr::NeuroChannelCommand c; c.modality=dnxr::FullDiveModality::Visual; c.intensity=1.0f; f.visual.channels.push_back(c); auto r=l.recordFrame(f,1.0f); assert(!r.withinLimits); assert(r.cooldownRequired); return 0; }
