#include "full_dive/safety/ModalityExposureLedger.h"
#include "neural_chip/NeuralVectorSlewRateLimiter.h"
#include <cassert>
int main(){ dnxr::FullDiveDosePolicy p; p.visual.maxInstantLoad01=0.2f; p.visual.maxDeltaPerFrame01=0.01f; dnxr::ModalityExposureLedger l; dnxr::FullDiveChipFrame f; f.visual.globalLuminance01=1.0f; auto out=l.apply(f,p); assert(out.visual.globalLuminance01<=0.21f); dnxr::NeuralVectorSlewRateLimiter s; s.apply(f,p); dnxr::FullDiveChipFrame f2=f; f2.visual.globalLuminance01=0.0f; auto limited=s.apply(f2,p); assert(limited.visual.globalLuminance01>=0.98f); }
