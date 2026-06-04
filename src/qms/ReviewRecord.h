#pragma once

#include <string>
#include <vector>

namespace dnxr::qms {

struct ReviewRecord {
    std::string id;
    std::string artifactId;
    std::string reviewer;
    std::string role;
    std::string date;
    std::vector<std::string> findings;
    bool accepted = false;

    bool isComplete() const;
};

} // namespace dnxr::qms
