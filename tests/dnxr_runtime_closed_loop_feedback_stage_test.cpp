#include "runtime/RuntimeClosedLoopFeedbackStage.h"
#include <cassert>
int main(){ dnxr::RuntimePipelineFrame f; f.chipFrame.channels={0.1f}; f.chipFrame.signalQuality=0.9f; f.stimulusCommand.boundedChannels={0.1f}; f.stimulusCommand.riskScore=0.1f; auto o=dnxr::RuntimeClosedLoopFeedbackStage::execute(f); assert(o.closedLoopFeedback.safetyVerified); return 0; }
