#pragma once

#include "../runtime/RuntimeFrame.h"

#include <string>
#include <vector>

namespace dnxr {

class ReplayRecorder {
public:
    void record(const RuntimeFrame& frame);
    bool saveToFile(const std::string& path) const;
    const std::vector<RuntimeFrame>& frames() const;
    void clear();

private:
    std::vector<RuntimeFrame> frames_;
};

} // namespace dnxr
