#include "RuntimeProfiler.h"
namespace dnxr { void RuntimeProfiler::beginCycle() noexcept { tracker_.begin(); } double RuntimeProfiler::endCycle() noexcept { return tracker_.end(); } std::string RuntimeProfiler::report() const { return tracker_.metrics().summary(); } }
