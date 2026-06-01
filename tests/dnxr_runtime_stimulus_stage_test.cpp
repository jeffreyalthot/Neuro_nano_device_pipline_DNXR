#include "../src/runtime/RuntimeStimulusStage.h"
#include <cassert>
int main() { assert(dnxr::RuntimeStimulusStage::validate(true, "bad").continueCycle); return 0; }
