#include "../src/runtime/RuntimeSignalStage.h"
#include <cassert>
int main() { auto status = dnxr::RuntimeSignalStage::validate(true, "bad"); assert(status.reason == "signal_ok"); return 0; }
