#pragma once
namespace dnxr { class HardwareBenchEmergencyStop { public: bool verify(){ ready_=true; return true; } bool ready() const { return ready_; } private: bool ready_=false; }; }
