#include "performance/PipelineBenchmark.h"
#include <cassert>
int main(){ dnxr::PipelineBenchmark b; auto r=b.run(3,[](std::uint64_t){}); assert(r.find("samples=3")!=std::string::npos); return 0; }
