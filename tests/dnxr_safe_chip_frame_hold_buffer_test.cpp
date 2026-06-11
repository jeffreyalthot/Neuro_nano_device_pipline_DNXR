#include "neural_chip/SafeChipFrameHoldBuffer.h"
#include <cassert>
int main(){ dnxr::SafeChipFrameHoldBuffer b; dnxr::FullDiveChipFrame f; f.visual.globalLuminance01=0.8f; f.auditory.loudness01=0.6f; b.rememberAccepted(f); auto d=b.makeDecompressionFrame(2,20,0.5f); assert(d.frameId==2); assert(d.visual.globalLuminance01<=0.4f); assert(d.olfactory.odorPresence01==0.0f); assert(d.motorIntent.stopIntent01==1.0f); }
