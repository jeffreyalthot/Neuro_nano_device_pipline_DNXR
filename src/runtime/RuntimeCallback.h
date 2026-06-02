#pragma once

#include <functional>

namespace dnxr {

template <typename Signature>
using RuntimeCallback = std::function<Signature>;

} // namespace dnxr
