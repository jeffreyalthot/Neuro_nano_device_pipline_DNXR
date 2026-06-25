#pragma once
#include <string>
namespace dnxr { class NeuralChipPhantomInterface { public: std::string targetType() const { return "PHANTOM"; } bool connected() const { return connected_; } void connect(){ connected_=true; } private: bool connected_=false; }; }
