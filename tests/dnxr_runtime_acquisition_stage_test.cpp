#include "../src/runtime/RuntimeAcquisitionStage.h"
#include <cassert>
int main() { assert(dnxr::RuntimeAcquisitionStage::validate(true, "empty").continueCycle); assert(!dnxr::RuntimeAcquisitionStage::validate(false, "empty").continueCycle); return 0; }
