#include "full_dive/chip_pipeline/ClosedLoopPerceptionErrorController.h"
#include <cassert>
int main(){ dnxr::ClosedLoopPerceptionErrorController c; dnxr::FullDiveChipFrame f; f.visual.motionFlow01=1.0f; f.visual.flickerRisk01=0.7f; dnxr::ChipReadFrame r; r.signalQuality01=0.1f; auto e=c.analyze(f,r); auto correction=c.decide(e); assert(correction.visualGain01<1.0f); auto out=c.applyCorrection(f,correction); assert(out.visual.motionFlow01<1.0f); }
