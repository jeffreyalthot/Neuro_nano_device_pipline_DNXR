#pragma once

#include "../runtime/RuntimeFrame.h"

#include <cstddef>
#include <string>
#include <vector>

namespace dnxr {

class ReplayPlayer {
public:
    bool loadFromFile(const std::string& path);
    bool hasNext() const;
    RuntimeFrame next();
    void reset();
    std::size_t size() const;
    const std::vector<RuntimeFrame>& frames() const;
    const std::vector<std::string>& frameHashes() const;

private:
    std::vector<RuntimeFrame> frames_;
    std::vector<std::string> frameHashes_;
    std::size_t cursor_ = 0U;
};

} // namespace dnxr
