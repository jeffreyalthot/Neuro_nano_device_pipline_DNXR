#pragma once
#include <string>
namespace dnxr { struct RNDTechnicalDossierInputs { std::string hilMetricsSummary="Pending bench/HIL metrics import."; std::string ctestSummary="Pending full CTest execution."; std::string staticChecksSummary="Pending static validation."; }; class RNDTechnicalDossierBuilder { public: std::string buildMarkdown(const RNDTechnicalDossierInputs& inputs) const; }; }
