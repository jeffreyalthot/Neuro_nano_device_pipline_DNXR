# Avatar neural feedback control

DNXR controls the game avatar from interpreted neural feedback and motor intent. This path is intentionally avatar-only: it maps intention to virtual movement and pose corrections, not to forced movement of a real body.

## Runtime path

```text
ChipReadFrame
  -> MotorCortexReadFrameAdapter
  -> MotorIntentDecoder
  -> MotorIntentConfidenceGate
  -> AvatarKinematicRigController::applyClosedLoop
  -> AvatarSafetyPoseController when required
  -> AvatarRigCommandFrame
```

`applyClosedLoop` blends decoded motor intent with neural feedback quality and feedback motor vectors. The result drives a full-body avatar rig covering locomotion, pelvis/spine/chest/neck/head, gaze, jaw, shoulders, elbows, wrists, hands, individual fingers, hips, knees, ankles, feet, and toes.

## Safety boundary

The controller must remain a virtual avatar controller. If confidence is low, stop intent is high, tremor/spasm/fatigue risk is high, emergency latch is active, or the session supervisor requests hold, the avatar is moved into a safety pose instead of amplifying movement.
