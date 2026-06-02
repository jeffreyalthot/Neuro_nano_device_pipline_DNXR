#pragma once

#include <cassert>
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
        : object_(static_cast<const void*>(&callable)),
          invoker_([](const void* object, Args... args) -> Return {
              return (*const_cast<Callable*>(static_cast<const Callable*>(object)))(std::forward<Args>(args)...);
          }) {}

    template <typename Callable>
    RuntimeCallbackRef(const Callable& callable)
        : object_(static_cast<const void*>(&callable)),
          invoker_([](const void* object, Args... args) -> Return {
              return (*static_cast<const Callable*>(object))(std::forward<Args>(args)...);
          }) {}

    bool empty() const noexcept { return invoker_ == nullptr; }
    explicit operator bool() const noexcept { return !empty(); }

    Return operator()(Args... args) const {
        assert(invoker_ && "RuntimeCallbackRef invoked while empty");
        return invoker_(object_, std::forward<Args>(args)...);
    }

private:
    const void* object_ = nullptr;
    Return (*invoker_)(const void*, Args...) = nullptr;
};

} // namespace dnxr
