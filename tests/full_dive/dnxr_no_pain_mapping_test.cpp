#include "full_dive/FiveSenseFusionModel.h"
#include <cassert>
int main(){ dnxr::FiveSenseNeuralState s; auto o=dnxr::FiveSenseFusionModel().fuse(s); assert(o.globalPerceptualCoherence>=0.0f); return 0; }
