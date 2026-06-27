#include "PipelineBenchmark.h"
namespace dnxr { std::string PipelineBenchmark::run(std::uint64_t cycles, const std::function<void(std::uint64_t)>& step) { RuntimeProfiler profiler; for (std::uint64_t i=0;i<cycles;++i){ profiler.beginCycle(); step(i); profiler.endCycle(); } return profiler.report(); } }
