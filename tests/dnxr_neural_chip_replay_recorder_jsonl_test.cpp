#include "neural_chip/NeuralChipReplayRecorder.h"
#include <cassert>
#include <fstream>
#include <string>
int main(){ dnxr::NeuralChipReplayRecorder r; dnxr::NeuralChipReplayRecord rec; rec.frameId=1; rec.timestampMs=1; rec.commitResult.accepted=true; rec.decisionTrace.push_back({1,"Stage","visual","visual.x","Rule",0,0,0}); r.record(rec); assert(r.exportJsonl("/tmp/dnxr_replay.jsonl")); std::ifstream in("/tmp/dnxr_replay.jsonl"); std::string s; std::getline(in,s); assert(s.find("frameId")!=std::string::npos); assert(s.find("payload")==std::string::npos); }
