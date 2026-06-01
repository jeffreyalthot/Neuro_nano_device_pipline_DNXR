#pragma once

#include "IInputSignalProvider.h"
#include "../config/RuntimeConfig.h"

#include <memory>

namespace dnxr {

class RuntimeSignalRouter {
public:
    explicit RuntimeSignalRouter(const RuntimeConfig& config);

    std::vector<float> nextSignal(int cycle);
    std::string activeSource() const;

private:
    std::unique_ptr<IInputSignalProvider> provider_;
};

} // namespace dnxr
