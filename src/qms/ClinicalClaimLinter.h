#pragma once
#include <string>
#include <vector>
namespace dnxr { struct ClinicalClaimLintResult { bool allowed=true; std::vector<std::string> blockedTerms; }; class ClinicalClaimLinter { public: ClinicalClaimLintResult lint(const std::string& text) const; }; }
