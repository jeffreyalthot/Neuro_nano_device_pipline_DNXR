#include "neural_chip/NeuralChipReplayRecorder.h"
#include <cassert>
int main(){ dnxr::ReplayRecorderPolicy p; p.maxInMemoryRecords=2; dnxr::NeuralChipReplayRecorder r(p); dnxr::NeuralChipReplayRecord rec; rec.frameId=1; r.record(rec); rec.frameId=2; r.record(rec); rec.frameId=3; r.record(rec); assert(r.records().size()==2); assert(r.records().front().frameId==2); p.dropOldest=false; r.setPolicy(p); rec.frameId=4; r.record(rec); assert(r.records().back().frameId==3); }
