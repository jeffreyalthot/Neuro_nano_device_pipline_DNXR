#pragma once
#include "PipelineStepResult.h"
#include <string>
#include <vector>
namespace dnxr { class RuntimeDiagnostics { public: void record(PipelineStepResult result); const std::vector<PipelineStepResult>& steps() const noexcept { return steps_; } std::string report() const; private: std::vector<PipelineStepResult> steps_; }; }
