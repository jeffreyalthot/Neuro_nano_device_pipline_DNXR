#pragma once
#include <map>
#include <set>
#include <string>
namespace dnxr { class CalibrationDependencyGraph { public: void addDependency(const std::string& modality,const std::string& dependency); bool dependenciesSatisfied(const std::string& modality,const std::set<std::string>& calibrated) const; private: std::map<std::string,std::set<std::string>> dependencies_; }; }
