#include "neural_chip/PipelineSafetyCaseReport.h"
#include <cassert>
int main(){ dnxr::NeuralChipReplayRecorder rr; dnxr::NeuralChipReplayRecord r; r.frameId=1; r.pipelinePathKind=dnxr::PipelinePathKind::AckMissingHold; r.sessionDigestHash=42; rr.record(r); auto report=dnxr::PipelineSafetyCaseReportBuilder().build(rr,true,true,true,true); assert(report.totalFrames==1); assert(report.ackHolds==1); assert(report.replayDigestPresent); }
