#pragma once
#include <string>
namespace dnxr { struct ReplayConsistencyReport { bool consistent = true; std::string expected; std::string actual; std::string explain() const; }; }
