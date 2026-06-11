#include "neural_chip/DisabledModalityNeutralityValidator.h"
#include <cassert>
int main(){ dnxr::FullDiveChipFrame f; dnxr::ChipCapabilities c; c.canVisual=false; f.visual.globalLuminance01=0.1f; assert(!dnxr::DisabledModalityNeutralityValidator().validate(f,c).accepted); f.visual.globalLuminance01=0; assert(dnxr::DisabledModalityNeutralityValidator().validate(f,c).accepted); }
