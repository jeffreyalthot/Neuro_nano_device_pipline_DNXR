#pragma once
#include "RuntimeProfiler.h"
#include <cstdint>
#include <functional>
#include <string>
namespace dnxr { class PipelineBenchmark { public: std::string run(std::uint64_t cycles, const std::function<void(std::uint64_t)>& step); }; }
