#include "full_dive/safety/ModalityExposureLedger.h"
#include <cassert>

int main() {
    dnxr::FullDiveChipFrame frame;
    frame.visual.globalLuminance01 = 0.8f;
    frame.auditory.loudness01 = 0.8f;
    frame.olfactory.odorPresence01 = 0.2f;
    frame.olfactory.nauseaRisk01 = 0.5f;
    frame.motorIntent.spasmRisk01 = 0.6f;

    dnxr::ModalityExposureLedger ledger;
    const auto out = ledger.apply(frame);
    assert(ledger.state().visual.rollingLoad01 > 0.0f);
    assert(ledger.state().olfactory.hold);
    assert(out.olfactory.odorPresence01 == 0.0f);
    assert(out.motorIntent.stopIntent01 == 1.0f);
    return 0;
}
