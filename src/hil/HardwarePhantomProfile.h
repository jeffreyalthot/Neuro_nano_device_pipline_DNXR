#pragma once
#include <string>
namespace dnxr { struct HardwarePhantomProfile { std::string profileId="PHANTOM-DEFAULT"; bool connected=true; double maxStimulusLoad=1.0; double latencyBudgetMs=20.0; }; }
