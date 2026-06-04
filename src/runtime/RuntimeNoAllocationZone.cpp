#include "RuntimeNoAllocationZone.h"
namespace dnxr { void RuntimeNoAllocationZone::enter(std::size_t a){entryAllocations_=a;active_=true;} bool RuntimeNoAllocationZone::exitWithoutGrowth(std::size_t a){bool ok=active_ && a==entryAllocations_; active_=false; return ok;} }
