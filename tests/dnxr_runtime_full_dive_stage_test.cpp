#include "../src/runtime/RuntimeFullDiveStage.h"
#include <cassert>
int main() { assert(dnxr::RuntimeFullDiveStage::validate(false, "full_dive_stage_disabled").reason == "full_dive_stage_disabled"); return 0; }
