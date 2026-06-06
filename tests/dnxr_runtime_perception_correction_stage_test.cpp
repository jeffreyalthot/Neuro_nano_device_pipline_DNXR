#include "runtime/RuntimePerceptionCorrectionStage.h"
#include <cassert>
int main(){ dnxr::RuntimePipelineFrame f; f.closedLoopFeedback.adjustment=0.01f; auto o=dnxr::RuntimePerceptionCorrectionStage::execute(f); assert(o.continueCycle); return 0; }
