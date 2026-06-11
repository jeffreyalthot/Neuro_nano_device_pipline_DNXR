#include "neural_chip/PipelineResumeController.h"
#include <cassert>
int main(){ dnxr::PipelineResumeController r; assert(r.normal()); r.lock(); assert(r.locked()); r.beginControlledResume(); assert(!r.locked()); auto d=r.decide(); assert(d.phase==dnxr::ResumePhase::MinimalHold); assert(!d.allowNormalFrame); }
