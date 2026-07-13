#include "../src/runtime/RuntimePipelineRunner.h"

#include <cassert>
#include <type_traits>

int main() {
    static_assert(!std::is_same<dnxr::RuntimePipelineRunner::RuntimePipelineRunInputRef,
                                dnxr::RuntimePipelineRunner::RuntimePipelineRunInput>::value,
                  "The ref input is separate from the transitional std::function sink input");
    dnxr::RuntimePipelineRunner runner;
    (void)runner;
    assert(true);
    return 0;
}
