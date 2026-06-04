#pragma once
#include <cstddef>
namespace dnxr { class RuntimeNoAllocationZone { public: void enter(std::size_t allocations); bool exitWithoutGrowth(std::size_t allocations); private: std::size_t entryAllocations_=0; bool active_=false; }; }
