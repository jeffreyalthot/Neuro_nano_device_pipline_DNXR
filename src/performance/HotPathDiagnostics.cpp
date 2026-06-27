#include "HotPathDiagnostics.h"
namespace dnxr { std::string HotPathDiagnostics::summary() const { return std::string("allocation_guard=") + (allocationGuardEnabled?"on":"off") + " blocking_log_avoided=" + (blockingLogAvoided?"yes":"no"); } }
