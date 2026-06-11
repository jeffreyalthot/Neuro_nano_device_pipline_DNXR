#include "neural_chip/NeuralChipReplayRecorder.h"
#include "neural_chip/NeuralChipReplayPlayer.h"
#include <cassert>
#include <fstream>
int main(){ dnxr::NeuralChipReplayRecorder rr; dnxr::NeuralChipReplayRecord r; r.frameId=1; r.timestampMs=1; r.commitResult.accepted=true; r.decisionTrace.push_back({1,"Stage","m","f","r",0,0,0}); rr.record(r); assert(rr.exportJsonl("/tmp/dnxr_schema.jsonl")); assert(dnxr::NeuralChipReplayPlayer().validateJsonl("/tmp/dnxr_schema.jsonl").valid); {std::ofstream o("/tmp/dnxr_bad_schema.jsonl"); o<<"{\"schemaVersion\":\"bad\",\"frameId\":1,\"timestampMs\":1,\"commitAccepted\":true,\"safetyHold\":false,\"decisionTraceCount\":1,\"pipelinePathKind\":\"NormalCommit\",\"pipelineState\":\"Running\"}\n";} assert(!dnxr::NeuralChipReplayPlayer().validateJsonl("/tmp/dnxr_bad_schema.jsonl").valid); }
