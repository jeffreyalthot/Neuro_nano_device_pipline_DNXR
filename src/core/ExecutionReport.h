#pragma once
#include <string>
#include <vector>
namespace dnxr { struct ExecutionReport { std::string mode; std::vector<std::string> diagnostics; std::string render() const; }; }
