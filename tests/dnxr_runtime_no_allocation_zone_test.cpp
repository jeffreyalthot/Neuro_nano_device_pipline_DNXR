#include "../src/runtime/RuntimeNoAllocationZone.h"
#include <cassert>
int main(){ dnxr::RuntimeNoAllocationZone z; z.enter(100); assert(z.exitWithoutGrowth(100)); z.enter(100); assert(!z.exitWithoutGrowth(101)); return 0; }
