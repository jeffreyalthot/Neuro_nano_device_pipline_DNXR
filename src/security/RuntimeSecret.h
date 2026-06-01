#pragma once

#include <string>

namespace dnxr {

struct RuntimeSecret {
    std::string id;
    std::string value;
    bool simulationGenerated = false;

    bool empty() const { return value.empty(); }
};

} // namespace dnxr
