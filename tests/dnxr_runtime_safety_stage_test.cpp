#include "../src/runtime/RuntimeSafetyStage.h"
#include <cassert>
int main() { assert(dnxr::RuntimeSafetyStage::validate(true, "unsafe").reason == "safety_ok"); return 0; }
