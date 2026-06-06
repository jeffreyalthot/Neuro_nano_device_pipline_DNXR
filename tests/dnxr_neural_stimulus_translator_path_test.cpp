#include "full_dive/NeuralStimulusTranslator.h"
#include <cassert>
int main(){ dnxr::FullDiveNeuralFrame f; f.sequenceId=7; f.safety.patientCalibrationHash="cal"; f.safety.emergencyCutoffAvailable=true; f.safety.aggregateRisk=0.1f; dnxr::NeuroChannelCommand c; c.modality=dnxr::FullDiveModality::Visual; c.targetRegion="VISUAL_PRIMARY"; c.calibrationProfileId="cal"; c.intensity=0.2f; c.safetyLimit=0.3f; f.visual.channels.push_back(c); auto r=dnxr::NeuralStimulusTranslator().translate(f); assert(r.valid); assert(r.command.sequenceId==7); return 0; }
