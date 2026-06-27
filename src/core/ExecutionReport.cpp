#include "ExecutionReport.h"
#include <sstream>
namespace dnxr { std::string ExecutionReport::render() const { std::ostringstream out; out << "ExecutionReport mode=" << mode; for (const auto& d: diagnostics) out << "\n" << d; return out.str(); } }
