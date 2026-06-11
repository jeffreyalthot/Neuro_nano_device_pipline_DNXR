#include "full_dive/chip_pipeline/WorldFrameSanitizer.h"
#include <cassert>
#include <cmath>
int main(){ dnxr::NeuralChipOnlyPipelineConfig cfg=dnxr::NeuralChipOnlyPipelineConfig::simulationSafeDefaults(); dnxr::NeuralChipOnlyWorldFrame w; w.frameId=1; w.timestampMs=-1; auto r=dnxr::WorldFrameSanitizer{}.sanitize(w,cfg); assert(!r.accepted); assert(r.fatalLifecycleInput); w.timestampMs=NAN; r=dnxr::WorldFrameSanitizer{}.sanitize(w,cfg); assert(!r.accepted); w.timestampMs=1; w.frameId=0; r=dnxr::WorldFrameSanitizer{}.sanitize(w,cfg); assert(!r.accepted); w.frameId=2; w.audio.loudness01=NAN; r=dnxr::WorldFrameSanitizer{}.sanitize(w,cfg); assert(r.accepted); assert(r.sanitized.audio.loudness01==0.0f); }
