#include "../src/runtime/RuntimeCallbackRef.h"

#include <cassert>
#include <string>

struct CallableObject {
    int factor = 3;
    int operator()(int value) const { return value * factor; }
};

int main() {
    dnxr::RuntimeCallbackRef<void()> empty;
    assert(empty.empty());
    assert(!empty);

    int value = 0;
    auto lambda = [&]() { value += 2; };
    dnxr::RuntimeCallbackRef<void()> lambdaRef(lambda);
    assert(lambdaRef);
    lambdaRef();
    assert(value == 2);

    auto mutableLambda = [count = 0]() mutable { return ++count; };
    dnxr::RuntimeCallbackRef<int()> mutableRef(mutableLambda);
    assert(mutableRef() == 1);
    assert(mutableRef() == 2);

    CallableObject callable{4};
    dnxr::RuntimeCallbackRef<int(int)> objectRef(callable);
    assert(objectRef(5) == 20);

    const auto constLambda = [](const std::string& text) { return text.size(); };
    dnxr::RuntimeCallbackRef<std::size_t(const std::string&)> constRef(constLambda);
    assert(constRef("dnxr") == 4U);

    return 0;
}
