#pragma once
#include <string>
namespace dnxr { struct ThermalRequest { float warmth=0,cold=0,gradient=0; }; struct ThermalResult { float warmth=0,cold=0,adaptation=0; bool burnRiskLockout=false; std::string reason="ok"; }; class ThermalPerceptionEngine { public: ThermalResult update(const ThermalRequest& r, double dtMs); private: float adaptation_=0; }; }
