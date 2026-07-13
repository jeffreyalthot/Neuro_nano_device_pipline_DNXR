#include "../src/runtime/RuntimePipelineRunInput.h"

#include <cassert>
#include <type_traits>

int main() {
    static_assert(!std::is_same<dnxr::RuntimePipelineRunInputRef,
                                dnxr::RuntimePipelineRunInput>::value,
                  "The ref input is separate from the transitional std::function sink input");
    assert(true);
    return 0;
}
