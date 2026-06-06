#include "runtime/RuntimeBodySchemaStage.h"
#include <cassert>
int main(){ dnxr::RuntimePipelineFrame f; dnxr::BodyPoseState a,b; auto o=dnxr::RuntimeBodySchemaStage::execute(f,a,b); assert(o.bodySchemaReport.safe); return 0; }
