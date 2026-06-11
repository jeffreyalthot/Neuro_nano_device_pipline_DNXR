#include "neural_chip/FullDiveChipFrameDiff.h"
#include <cassert>

int main(){ dnxr::FullDiveChipFrame a,b; b.visual.globalLuminance01=0.2f; b.gustatory.bitter01=0.1f; auto d=dnxr::FullDiveChipFrameDiff::compare(a,b); assert(d.size()>=2); }
