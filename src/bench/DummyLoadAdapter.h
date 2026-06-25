#pragma once
#include <string>
namespace dnxr { class DummyLoadAdapter { public: explicit DummyLoadAdapter(std::string id="dummy_load") : id_(std::move(id)) {} bool attached() const { return attached_; } void attach(){ attached_=true; } void detach(){ attached_=false; } const std::string& id() const { return id_; } std::string targetType() const { return "DUMMY_LOAD"; } private: std::string id_; bool attached_=false; }; }
