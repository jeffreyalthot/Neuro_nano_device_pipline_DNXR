#pragma once

#include "../config/RuntimeConfig.h"

namespace dnxr {

class RuntimeApplication {
public:
    RuntimeApplication();
    explicit RuntimeApplication(RuntimeConfig config);

    int run();

private:
    RuntimeConfig config_;
};

} // namespace dnxr
