#include "full_dive/chip_pipeline/WorldFrameSanitizer.h"
#include <cassert>
#include <cmath>
int main(){ dnxr::NeuralChipOnlyWorldFrame w; w.timestampMs=-1; w.audio.loudness01=NAN; w.contacts.pressure01=INFINITY; w.odor.presence01=1.0f; auto o=dnxr::WorldFrameSanitizer().apply(w,dnxr::NeuralChipOnlyPipelineConfig::simulationSafeDefaults()); assert(o.timestampMs==0); assert(o.audio.loudness01==0); assert(o.contacts.pressure01==0); assert(o.odor.presence01==0); }
