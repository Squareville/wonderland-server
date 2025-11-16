#include "GhostComponent.h"
#include "PlayerManager.h"
#include "Character.h"
#include "ControllablePhysicsComponent.h"
#include "UserManager.h"
#include "User.h"

#include "Amf3.h"
#include "GameMessages.h"

GhostComponent::GhostComponent(Entity* parent, const int32_t componentID) : Component(parent, componentID) {
	m_GhostReferencePoint = NiPoint3Constant::ZERO;
	m_GhostOverridePoint = NiPoint3Constant::ZERO;
	m_GhostOverride = false;

	RegisterMsg(this, &GhostComponent::MsgGetObjectReportInfo);
}

GhostComponent::~GhostComponent() {
	for (auto& observedEntity : m_ObservedEntities) {
		if (observedEntity == LWOOBJID_EMPTY) continue;

		auto* entity = Game::entityManager->GetGhostCandidate(observedEntity);
		if (!entity) continue;

		entity->SetObservers(entity->GetObservers() - 1);
	}
}

void GhostComponent::LoadFromXml(const tinyxml2::XMLDocument& doc) {
	auto* objElement = doc.FirstChildElement("obj");
	if (!objElement) return;
	auto* ghstElement = objElement->FirstChildElement("ghst");
	if (!ghstElement) return;
	m_IsGMInvisible = ghstElement->BoolAttribute("i");
}

void GhostComponent::UpdateXml(tinyxml2::XMLDocument& doc) {
	auto* objElement = doc.FirstChildElement("obj");
	if (!objElement) return;
	auto* ghstElement = objElement->FirstChildElement("ghst");
	if (ghstElement) objElement->DeleteChild(ghstElement);
	// Only save if GM invisible
	if (!m_IsGMInvisible) return;
	ghstElement = objElement->InsertNewChildElement("ghst");
	if (ghstElement) ghstElement->SetAttribute("i", m_IsGMInvisible);
}

void GhostComponent::SetGhostReferencePoint(const NiPoint3& value) {
	m_GhostReferencePoint = value;
}

void GhostComponent::SetGhostOverridePoint(const NiPoint3& value) {
	m_GhostOverridePoint = value;
}

void GhostComponent::AddLimboConstruction(LWOOBJID objectId) {
	m_LimboConstructions.insert(objectId);
}

void GhostComponent::RemoveLimboConstruction(LWOOBJID objectId) {
	m_LimboConstructions.erase(objectId);
}

void GhostComponent::ConstructLimboEntities() {
	for (const auto& objectId : m_LimboConstructions) {
		auto* entity = Game::entityManager->GetEntity(objectId);
		if (!entity) continue;

		Game::entityManager->ConstructEntity(entity, m_Parent->GetSystemAddress());
	}

	m_LimboConstructions.clear();
}

void GhostComponent::ObserveEntity(LWOOBJID id) {
	m_ObservedEntities.insert(id);
}

bool GhostComponent::IsObserved(LWOOBJID id) {
	return m_ObservedEntities.contains(id);
}

void GhostComponent::GhostEntity(LWOOBJID id) {
	m_ObservedEntities.erase(id);
}

bool GhostComponent::MsgGetObjectReportInfo(GameMessages::GameMsg& msg) {
	auto& reportMsg = static_cast<GameMessages::GetObjectReportInfo&>(msg);
	auto& cmptType = reportMsg.info->PushDebug("Ghost");
	cmptType.PushDebug<AMFIntValue>("Component ID") = GetComponentID();
	cmptType.PushDebug<AMFBoolValue>("Is GM Invis") = false;

	return true;
}
