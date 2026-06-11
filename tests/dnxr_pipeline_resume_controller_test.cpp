#include "neural_chip/PipelineResumeController.h"
#include <cassert>
int main(){ dnxr::PipelineResumeController c; c.lock(); assert(!c.decide().allowNormalFrame); c.beginControlledResume(); assert(c.decide().phase==dnxr::ResumePhase::MinimalHold); assert(c.decide().phase==dnxr::ResumePhase::NeutralHold); assert(c.decide().phase==dnxr::ResumePhase::Decompression); auto r=c.decide(); assert(r.phase==dnxr::ResumePhase::ReducedGain && r.allowNormalFrame && r.globalGain01<=0.25f); c.decide(); assert(c.decide().phase==dnxr::ResumePhase::Normal); }
