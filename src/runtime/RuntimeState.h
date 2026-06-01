#pragma once

#include "RuntimeFrame.h"

#include <cstddef>
#include <vector>

namespace dnxr {

class RuntimeState {
public:
    explicit RuntimeState(std::size_t historyLimit = 256U);

    void commitFrame(const RuntimeFrame& frame);
    bool hasFrame() const;
    const RuntimeFrame& currentFrame() const;
    const std::vector<RuntimeFrame>& history() const;
    std::size_t historyLimit() const;
    void clear();

private:
    std::size_t historyLimit_;
    std::vector<RuntimeFrame> history_;
};

} // namespace dnxr
