#include "full_dive/safety/ExperimentalChemicalChannelGate.h"
#include <cassert>

int main(){
    dnxr::ExperimentalChemicalChannelGate gate; dnxr::FullDiveChipFrame f; f.olfactory.odorPresence01=0.8f; f.gustatory.bitter01=0.8f; auto off=gate.apply(f); assert(off.olfactory.odorPresence01==0.0f); assert(off.gustatory.bitter01==0.0f);
    gate.setEnabled(true); auto on=gate.apply(f); assert(on.olfactory.odorPresence01==0.15f); assert(on.gustatory.bitter01==0.05f); assert(on.gustatory.swallowingConflictRisk01==0.0f);
}
