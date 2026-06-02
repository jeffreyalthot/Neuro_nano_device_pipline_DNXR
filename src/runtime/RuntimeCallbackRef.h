#pragma once

#include <utility>

namespace dnxr {

template <typename Signature>
class RuntimeCallbackRef;

template <typename Return, typename... Args>
class RuntimeCallbackRef<Return(Args...)> {
public:
    RuntimeCallbackRef() = default;

    template <typename Callable>
    RuntimeCallbackRef(Callable& callable)
        : object_(static_cast<void*>(&callable)),
          invoker_([](void* object, Args... args) -> Return {
              return (*static_cast<Callable*>(object))(std::forward<Args>(args)...);
          }) {}

    bool empty() const noexcept { return invoker_ == nullptr; }
    explicit operator bool() const noexcept { return !empty(); }

    Return operator()(Args... args) const {
        return invoker_(object_, std::forward<Args>(args)...);
    }

private:
    void* object_ = nullptr;
    Return (*invoker_)(void*, Args...) = nullptr;
};

} // namespace dnxr
