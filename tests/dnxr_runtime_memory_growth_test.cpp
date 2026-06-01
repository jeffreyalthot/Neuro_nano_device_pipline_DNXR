#include "../src/runtime/RuntimeAcquisitionStage.h"
#include <cassert>
int main() { auto status = dnxr::RuntimeAcquisitionStage::validate(true, "bad"); assert(status.continueCycle); return 0; }
