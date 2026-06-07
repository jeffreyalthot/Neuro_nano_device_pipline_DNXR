#include "RuntimeBodyPoseExtractionStage.h"

namespace dnxr {

const char* RuntimeBodyPoseExtractionStage::name() { return "body_pose_extraction"; }

RuntimePipelineFrame RuntimeBodyPoseExtractionStage::execute(RuntimePipelineFrame frame,
                                                            const PerceivedBodyPoseEstimator& perceivedEstimator,
                                                            const VirtualBodyPoseProvider& avatarProvider) {
    frame.perceivedBodyPose = perceivedEstimator.estimate(frame.chipFrame, frame.cleanSignals, frame.features, frame.calibratedFeatures);
    frame.avatarBodyPose = avatarProvider.currentAvatarPose(frame);
    frame.bodyPoseExtractionComplete = PerceivedBodyPoseEstimator::isComplete(frame.perceivedBodyPose) &&
                                       VirtualBodyPoseProvider::isComplete(frame.avatarBodyPose);
    if (!frame.bodyPoseExtractionComplete) {
        frame.fullDiveNeuralFrame.proprioception.channels.clear();
        frame.fullDiveNeuralFrame.somatosensory.channels.clear();
        frame.fullDiveNeuralFrame.vestibular.channels.clear();
        frame.continueCycle = false;
        frame.emergencyActive = true;
        frame.stopReason = "body_pose_extraction_incomplete";
        frame.fullDiveStatus = RuntimeStageStatus::emergency(frame.stopReason);
    }
    return frame;
}

} // namespace dnxr
