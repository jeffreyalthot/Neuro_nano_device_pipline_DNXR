#include "config/RuntimeMode.h"
#include <cassert>
int main() { assert(dnxr::runtimeModeFromString("full-unlocked") == dnxr::RuntimeMode::FullUnlocked); assert(dnxr::isInspectionEnabled(dnxr::RuntimeMode::Diagnostic)); assert(dnxr::toString(dnxr::RuntimeMode::SafeTest) == "safe_test_mode"); return 0; }
