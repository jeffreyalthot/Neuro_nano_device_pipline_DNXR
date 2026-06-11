#pragma once
#include <string>
#include <vector>
namespace dnxr {
struct TraceabilityCoverageDecision { bool complete=false; std::size_t rowsChecked=0; std::vector<std::string> gaps; };
class TraceabilityCoverageGate { public: explicit TraceabilityCoverageGate(std::string matrixPath="docs/TRACEABILITY_MATRIX.csv"); TraceabilityCoverageDecision evaluate() const; private: std::string matrixPath_; };
}
