#pragma once

#include "hil/HILScenarioResult.h"

#include <string>
#include <vector>

namespace dnxr {

class HILSessionReport {
public:
    void addResult(const HILScenarioResult& result);
    std::vector<HILScenarioResult> results() const;
    bool allPassed() const;
    std::string toMarkdown() const;

private:
    std::vector<HILScenarioResult> results_;
};

} // namespace dnxr
