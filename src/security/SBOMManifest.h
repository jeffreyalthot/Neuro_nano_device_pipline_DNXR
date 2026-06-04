#pragma once
#include <string>
#include <vector>
namespace dnxr { struct SBOMComponent { std::string name; std::string version; std::string supplier; }; class SBOMManifest { public: void add(SBOMComponent component); std::vector<SBOMComponent> components() const; bool empty() const; private: std::vector<SBOMComponent> components_; }; }
