#pragma once
#include <string>
#include <vector>
namespace dnxr {
struct BodyPoseState { float headPose=0.0f, spinePose=0.0f, armPosition=0.0f, handState=0.0f, legState=0.0f, balance=1.0f, virtualBodyScale=1.0f, virtualLimbOwnership=1.0f; std::vector<float> contactPoints; };
struct BodySchemaReport { float bodyOwnershipScore=1.0f; float embodimentCoherenceScore=1.0f; float limbMismatchRisk=0.0f; float avatarDissociationRisk=0.0f; bool safe=true; std::string reason="body_schema_coherent"; };
class BodySchemaEngine { public: BodySchemaReport evaluate(const BodyPoseState& perceived, const BodyPoseState& avatar) const; };
} // namespace dnxr
