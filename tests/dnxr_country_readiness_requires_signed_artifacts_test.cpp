#include "../src/readiness/CanadaReadinessProfile.h"
#include <cassert>
int main(){ dnxr::CanadaReadinessProfile p; dnxr::EvidenceRepository repo; for(const auto& item:p.items()) p.setItemComplete(item.id,true); assert(!p.blocksClinicalSubmission()); assert(p.blocksClinicalSubmission(repo,"canada:")); for(const auto& item:p.items()) repo.add({"EV-"+item.id,"qms/canada/"+item.id+".md","canada:"+item.id,item.id,"H-001",dnxr::EvidenceReviewState::Signed,"regulatory reviewer","2026-06-04"}); assert(!p.blocksClinicalSubmission(repo,"canada:")); return 0; }
