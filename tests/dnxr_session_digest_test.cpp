#include "neural_chip/NeuralChipSessionDigest.h"
#include <cassert>
int main(){ dnxr::NeuralChipSessionDigest d; dnxr::NeuralChipReplayRecord r; r.frameId=1; r.timestampMs=1; r.commitResult.accepted=true; d.update(r); r.frameId=2; r.commitResult.accepted=false; d.update(r); assert(d.value().frameCount==2); assert(d.value().acceptedCount==1); assert(d.value().rejectedCount==1); assert(d.value().rollingHash!=1469598103934665603ULL); }
