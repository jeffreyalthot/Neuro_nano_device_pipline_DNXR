#pragma once
#include <string>
namespace dnxr { struct HotPathDiagnostics { bool allocationGuardEnabled = true; bool blockingLogAvoided = true; std::string summary() const; }; }
