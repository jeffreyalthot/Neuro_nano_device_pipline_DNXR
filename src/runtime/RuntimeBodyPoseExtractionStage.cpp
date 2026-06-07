#include "RuntimeBodyPoseExtractionStage.h"
#include "RuntimeStageServices.h"

namespace dnxr {

const char* RuntimeBodyPoseExtractionStage::name() { return "body_pose_extraction"; }

RuntimePipelineFrame RuntimeBodyPoseExtractionStage::execute(RuntimePipelineFrame frame,
                                                            const PerceivedBodyPoseEstimator& perceivedEstimator,
                                                            const VirtualBodyPoseProvider& avatarProvider) {
    const auto& sourceFrame = frame.acquisitionChipFrame.sequenceId ? frame.acquisitionChipFrame : frame.chipFrame;
    frame.perceivedBodyPose = perceivedEstimator.estimate(sourceFrame, frame.cleanSignals, frame.features, frame.calibratedFeatures);
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

RuntimePipelineFrame RuntimeBodyPoseExtractionStage::execute(RuntimePipelineFrame frame, RuntimeStageServices& services) {
    static PerceivedBodyPoseEstimator defaultPerceivedEstimator;
    static VirtualBodyPoseProvider defaultAvatarProvider;
    const auto& perceived = services.perceivedBodyPoseEstimator ? *services.perceivedBodyPoseEstimator : defaultPerceivedEstimator;
    const auto& avatar = services.virtualBodyPoseProvider ? *services.virtualBodyPoseProvider : defaultAvatarProvider;
    return execute(std::move(frame), perceived, avatar);
}

} // namespace dnxr
