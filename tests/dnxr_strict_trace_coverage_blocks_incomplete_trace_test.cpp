#include "neural_chip/ReplayConsistencyVerifier.h"
#include <cassert>
int main(){ dnxr::NeuralChipReplayRecorder rr; dnxr::NeuralChipReplayRecord r; r.frameId=1; r.pipelinePathKind=dnxr::PipelinePathKind::NormalCommit; r.pipelineState=dnxr::NeuralChipPipelineState::Running; r.decisionTrace.push_back({1,"OnlyOneStage","trace","probe","incomplete",1.0f,0.0f,0.0f}); r.traceCoverageComplete=false; r.traceCoverageMissingStage="NeuralChipFrameCommitter"; rr.record(r); auto report=dnxr::ReplayConsistencyVerifier().verify(rr); assert(!report.valid); }
