#pragma once
#include <string>
namespace dnxr { struct PipelineStepResult { bool accepted = true; std::string step; std::string reason = "accepted"; }; }
