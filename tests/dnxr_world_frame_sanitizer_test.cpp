#include "full_dive/chip_pipeline/WorldFrameSanitizer.h"
#include <cassert>
#include <cmath>
int main(){ dnxr::NeuralChipOnlyWorldFrame w; w.frameId=1; w.timestampMs=1; w.audio.loudness01=NAN; w.contacts.pressure01=INFINITY; w.odor.presence01=1.0f; auto result=dnxr::WorldFrameSanitizer().sanitize(w,dnxr::NeuralChipOnlyPipelineConfig::simulationSafeDefaults()); assert(result.accepted); auto o=result.sanitized; assert(o.timestampMs==1); assert(o.audio.loudness01==0); assert(o.contacts.pressure01==0); assert(o.odor.presence01==0); w.timestampMs=-1; assert(!dnxr::WorldFrameSanitizer().sanitize(w,dnxr::NeuralChipOnlyPipelineConfig::simulationSafeDefaults()).accepted); }
