#pragma once
#include <string>
#include <vector>
namespace dnxr { struct TraceabilityLink { std::string requirementId; std::string riskId; std::string controlId; std::string codeFile; std::string testId; std::string evidenceId; std::string signoffId; }; struct TraceabilityDecision { bool complete=false; std::vector<std::string> gaps; }; class RequirementTraceabilityEngine { public: TraceabilityDecision evaluate(const std::vector<TraceabilityLink>& links) const; }; }
