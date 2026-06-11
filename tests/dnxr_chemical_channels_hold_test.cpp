#include "full_dive/gustatory_chip/NeuralGustatoryEncoder.h"
#include "full_dive/olfactory_chip/NeuralOlfactoryEncoder.h"
#include "full_dive/safety/ModalityExposureLedger.h"
#include <cassert>

int main() {
    dnxr::PatientOlfactoryMap odorMap;
    odorMap.experimentalChannelEnabled = false;
    dnxr::VirtualOdorField odor;
    odor.presence01 = 1.0f;
    assert(dnxr::NeuralOlfactoryEncoder{}.encode(odor, odorMap, {}).odorPresence01 == 0.0f);

    odorMap.experimentalChannelEnabled = true;
    dnxr::OlfactorySafetyState odorSafety;
    odorSafety.nauseaRisk01 = 0.5f;
    dnxr::FullDiveChipFrame frame;
    frame.olfactory = dnxr::NeuralOlfactoryEncoder{}.encode(odor, odorMap, odorSafety);
    dnxr::ModalityExposureLedger ledger;
    const auto held = ledger.apply(frame);
    assert(held.olfactory.odorPresence01 == 0.0f);
    assert(ledger.state().olfactory.hold);

    dnxr::PatientGustatoryMap tasteMap;
    tasteMap.experimentalChannelEnabled = false;
    dnxr::VirtualTasteFrame taste;
    taste.sweet01 = 1.0f;
    assert(dnxr::NeuralGustatoryEncoder{}.encode(taste, tasteMap, {}).sweet01 == 0.0f);
    return 0;
}
