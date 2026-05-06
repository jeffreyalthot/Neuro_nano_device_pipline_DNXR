#include "core/SystemKernel.h"

int main() {
    // The application entry point initializes the kernel and starts one protected runtime cycle.
    dnxr::SystemKernel kernel;
    kernel.boot();
    return 0;
}
