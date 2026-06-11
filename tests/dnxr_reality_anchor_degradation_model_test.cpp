#include "full_dive/safety/RealityAnchorDegradationModel.h"
#include <cassert>
int main(){ dnxr::RealityAnchorDegradationModel m; dnxr::NeuralRealityAnchorVector a; a.externalWorldRecall01=0.8f; assert(m.decide(a).globalGain01==0.75f); a.externalWorldRecall01=0.7f; assert(m.decide(a).requireHold); a.externalWorldRecall01=0.6f; assert(m.decide(a).triggerEmergencyLatch); }
