#include "ReplayConsistencyReport.h"
namespace dnxr { std::string ReplayConsistencyReport::explain() const { return consistent ? "replay_consistent" : "replay_divergent expected=" + expected + " actual=" + actual; } }
