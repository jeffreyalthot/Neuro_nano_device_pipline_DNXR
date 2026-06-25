#include "../../src/hardware/NeuralChipHAL.h"
#include <type_traits>
int main(){ static_assert(!std::is_convertible<dnxr::RawSensoryPayload, dnxr::BoundedStimulusCommand>::value, "raw sensory payload must not convert to bounded stimulus command"); return 0; }
