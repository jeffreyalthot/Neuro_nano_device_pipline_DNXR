#include "../../src/qms/ClinicalBuildBoundary.h"
#include <cassert>
int main(){ dnxr::ClinicalBuildBoundary b; assert(!b.validateModule("ResearchFullDiveOrchestrator",true).allowed); return 0; }
