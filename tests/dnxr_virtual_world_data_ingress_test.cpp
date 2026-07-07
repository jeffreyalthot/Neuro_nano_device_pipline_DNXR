#include "immersion/VirtualWorldDataIngress.h"

#include <cassert>

int main() {
  dnxr::VirtualWorldIngressFrame input;
  input.frameId = 42;
  input.timestampMs = 700.0;
  input.view.exposure = 0.8f;
  input.view.contrast = 0.7f;
  input.visualField.nearDepthLayer = 0.9f;
  input.visualField.retinalStability = 0.95f;
  input.audio.volume01 = 0.6f;
  input.audio.directionLeftRight01 = 0.25f;
  input.audio.sourceDistance01 = 0.4f;
  input.touch.pressure01 = 0.3f;
  input.touch.texture01 = 0.5f;
  input.touch.footGroundContact01 = 0.7f;
  input.body.handConfidence01 = 0.8f;
  input.body.footConfidence01 = 0.75f;
  input.body.avatarOwnership01 = 0.9f;
  input.vestibular.angularVelocity01 = 0.2f;
  input.vestibular.balanceError01 = 0.1f;
  input.chemical.odorPresence01 = 0.4f;
  input.chemical.sweet01 = 0.2f;
  input.realityAnchor.orientation01 = 1.0f;
  input.realityAnchor.identityContinuity01 = 0.95f;
  input.realityAnchor.sessionBoundary01 = 1.0f;
  input.realityAnchor.externalWorldRecall01 = 0.9f;

  const auto world = dnxr::VirtualWorldDataIngress{}.ingest(input);
  assert(world.frameId == 42);
  assert(world.timestampMs == 700.0);
  assert(world.scene.luminance01 == 0.8f);
  assert(world.scene.nearObstacle01 == 0.9f);
  assert(world.scene.horizonStability01 == 0.95f);
  assert(world.audio.loudness01 == 0.6f);
  assert(world.audio.leftRight01 == 0.25f);
  assert(world.contacts.pressure01 == 0.3f);
  assert(world.contacts.texture01 == 0.5f);
  assert(world.pose.handConfidence01 == 0.8f);
  assert(world.pose.ownership01 == 0.9f);
  assert(world.motion.angularVelocity01 == 0.2f);
  assert(world.motion.balanceError01 == 0.1f);
  assert(world.odor.presence01 == 0.4f);
  assert(world.taste.sweet01 == 0.2f);
  assert(world.realityAnchor.externalWorldRecall01 == 0.9f);
  assert(world.latencyBudget.visualLatencyMs == 16.67f);
  return 0;
}
