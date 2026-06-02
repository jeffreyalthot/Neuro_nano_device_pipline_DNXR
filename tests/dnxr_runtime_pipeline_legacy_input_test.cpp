#include "../src/runtime/RuntimePipelineRunner.h"

#include <cassert>
#include <type_traits>

int main() {
    static_assert(std::is_same<dnxr::RuntimePipelineRunner::RunInput,
                               dnxr::RuntimePipelineRunner::LegacyRunInput>::value,
                  "RunInput remains a temporary legacy compatibility alias");
    static_assert(!std::is_same<dnxr::RuntimePipelineRunner::RuntimePipelineRunInput,
                                dnxr::RuntimePipelineRunner::LegacyRunInput>::value,
                  "The services-only input is intentionally separate from legacy StageInputs");

    dnxr::RuntimePipelineRunner runner;
    (void)runner;
    assert(true);
    return 0;
}
