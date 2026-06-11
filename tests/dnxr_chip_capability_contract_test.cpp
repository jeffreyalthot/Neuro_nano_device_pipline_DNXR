#include "neural_chip/ChipCapabilityContract.h"
#include <cassert>
int main(){ dnxr::ChipCapabilities caps; caps.canAuditory=true; caps.supportsClosedLoopFeedback=false; caps.supportsHardwareCutoff=true; dnxr::FullDiveChipFrame f; f.visual.globalLuminance01=1.0f; f.auditory.loudness01=1.0f; f.motorIntent.confidence01=1.0f; auto out=dnxr::ChipCapabilityContract{caps}.apply(f); assert(out.visual.globalLuminance01==0.0f); assert(out.auditory.loudness01==1.0f); assert(out.motorIntent.confidence01<=0.25f); }
