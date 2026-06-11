#include "neural_chip/FullDiveChipFrameDiff.h"
#include <cassert>
#include <string>
int main(){ dnxr::FullDiveChipFrame a,b; b.visual.globalLuminance01=0.2f; auto d=dnxr::FullDiveChipFrameDiff::compare(a,b); assert(!d.empty()); assert(std::string(d[0].modality)=="visual"); assert(std::string(d[0].fieldPath)=="visual.globalLuminance01"); }
