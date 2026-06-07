#include "../../src/runtime/RuntimeBodyPoseExtractionStage.h"
#include <cassert>
int main(){ dnxr::RuntimePipelineFrame f; f.chipFrame.sequenceId=1; f.chipFrame.channels={0.0f}; f.chipFrame.signalQuality=0.0f; f.acquisitionChipFrame.sequenceId=2; f.acquisitionChipFrame.channels={0.2f}; f.acquisitionChipFrame.signalQuality=0.9f; f.cleanSignals={0.1f}; f.features={0.1f}; f.calibratedFeatures={0.1f,0.2f,0.3f,0.4f,1.0f,0.5f}; auto o=dnxr::RuntimeBodyPoseExtractionStage::execute(f); assert(o.perceivedBodyPose.virtualLimbOwnership>0.8f); return 0; }
