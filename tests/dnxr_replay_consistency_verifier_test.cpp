#include "neural_chip/ReplayConsistencyVerifier.h"
#include <cassert>
int main(){ dnxr::NeuralChipReplayRecorder rr; dnxr::NeuralChipReplayRecord r; r.frameId=1; r.timestampMs=1; r.commitResult.accepted=true; r.commitResult.safetyHold=false; r.decisionTrace.push_back({1,"Stage","m","f","r",0,0,0}); r.sessionDigestFrameCount=1; rr.record(r); auto report=dnxr::ReplayConsistencyVerifier().verify(rr); assert(report.valid); }
