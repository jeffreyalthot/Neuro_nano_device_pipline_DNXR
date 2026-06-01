#include "../src/runtime/RuntimeStimulusStage.h"
#include <cassert>
int main() { auto status = dnxr::RuntimeStimulusStage::validate(false, "stimulus_stage_unavailable"); assert(!status.continueCycle); return 0; }
