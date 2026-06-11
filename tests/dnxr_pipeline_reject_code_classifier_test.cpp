#include "neural_chip/PipelinePathClassifier.h"
#include <cassert>
int main(){ dnxr::PipelinePathClassifier c; assert(c.classify(dnxr::PipelineRejectCode::TraceCoverageFailed)==dnxr::PipelinePathKind::TraceCoverageReject); assert(c.classify(dnxr::PipelineRejectCode::ReentrantStep)==dnxr::PipelinePathKind::ReentrantStepReject); }
