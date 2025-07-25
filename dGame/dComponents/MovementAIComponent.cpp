#include "MovementAIComponent.h"

#include <utility>
#include <cmath>

#include "SkillComponent.h"
#include "ControllablePhysicsComponent.h"
#include "InventoryComponent.h"
#include "RenderComponent.h"
#include "BaseCombatAIComponent.h"
#include "dpCommon.h"
#include "dpWorld.h"
#include "EntityManager.h"
#include "SimplePhysicsComponent.h"
#include "CDClientManager.h"
#include "Game.h"
#include "dZoneManager.h"
#include "eWaypointCommandType.h"

#include "CDComponentsRegistryTable.h"
#include "QuickBuildComponent.h"
#include "CDPhysicsComponentTable.h"

#include "dNavMesh.h"
#include "StringifiedEnum.h"

namespace {
	/**
	 * Cache of all lots and their respective speeds
	 */
	std::map<LOT, float> m_PhysicsSpeedCache;
}

MovementAIComponent::MovementAIComponent(Entity* parent, MovementAIInfo info) : Component(parent) {
	m_Info = info;
	m_AtFinalWaypoint = true;

	m_BaseCombatAI = nullptr;

	m_BaseCombatAI = m_Parent->GetComponent<BaseCombatAIComponent>();

	//Try and fix the insane values:
	if (m_Info.wanderRadius > 5.0f) m_Info.wanderRadius *= 0.5f;
	if (m_Info.wanderRadius > 8.0f) m_Info.wanderRadius = 8.0f;
	if (m_Info.wanderSpeed > 0.5f) m_Info.wanderSpeed *= 0.5f;

	m_BaseSpeed = GetBaseSpeed(m_Parent->GetLOT());

	m_NextWaypoint = m_Parent->GetPosition();
	m_Acceleration = 0.4f;
	m_PullingToPoint = false;
	m_PullPoint = NiPoint3Constant::ZERO;
	m_HaltDistance = 0;
	m_TimeToTravel = 0;
	m_TimeTravelled = 0;
	m_CurrentSpeed = 0;
	m_MaxSpeed = 0;
	m_LockRotation = false;
	m_Path = nullptr;
	m_SourcePosition = m_Parent->GetPosition();
	m_Paused = false;
	m_SavedVelocity = NiPoint3Constant::ZERO;
	m_IsBounced = false;

	if (!m_Parent->GetComponent<BaseCombatAIComponent>()) SetPath(m_Parent->GetVarAsString(u"attached_path"));
}

void MovementAIComponent::SetPath(const std::string pathName) {
	m_Path = Game::zoneManager->GetZone()->GetPath(pathName);
	if (!pathName.empty()) LOG("WARNING: %s path %s", m_Path ? "Found" : "Failed to find", pathName.c_str());
	if (!m_Path) return;
	const auto waypointStart = m_Parent->GetVarAs<uint32_t>(u"attached_path_start");
	SetMaxSpeed(1);
	SetCurrentSpeed(m_BaseSpeed);
	SetPath(m_Path->pathWaypoints, waypointStart);
}

void MovementAIComponent::Pause(const float pauseTime) {
	if (m_Paused) return;
	m_Paused = true;
	m_Delay = pauseTime;
	SetPosition(ApproximateLocation());
	m_SavedVelocity = GetVelocity();
	SetVelocity(NiPoint3Constant::ZERO);
	Game::entityManager->SerializeEntity(m_Parent);
}

void MovementAIComponent::Resume() {
	if (!m_Paused) return;
	m_Paused = false;
	SetVelocity(m_SavedVelocity);
	m_SavedVelocity = NiPoint3Constant::ZERO;
	SetRotation(NiQuaternion::LookAt(m_Parent->GetPosition(), m_NextWaypoint));
	Game::entityManager->SerializeEntity(m_Parent);
}

void MovementAIComponent::Update(const float deltaTime) {
	bool wasPaused = m_Delay > 0.0f;
	m_Delay -= deltaTime;
	if (m_Delay > 0.0f) return;
	
	if (wasPaused) {
		Resume();
		return;
	}

	if (m_Paused) return;

	auto* const quickBuildComponent = m_Parent->GetComponent<QuickBuildComponent>();
	if (quickBuildComponent && quickBuildComponent->GetState() != eQuickBuildState::COMPLETED) return;

	if (m_PullingToPoint) {
		const auto source = GetCurrentWaypoint();

		const auto speed = deltaTime * 2.5f;

		NiPoint3 velocity = (m_PullPoint - source) * speed;

		SetPosition(source + velocity);

		if (Vector3::DistanceSquared(m_Parent->GetPosition(), m_PullPoint) < std::pow(2, 2)) {
			m_PullingToPoint = false;
		}

		return;
	}

	// Are we done?
	if (AtFinalWaypoint()) return;

	if (m_HaltDistance > 0) {
		// Prevent us from hugging the target
		if (Vector3::DistanceSquared(ApproximateLocation(), GetDestination()) < std::pow(m_HaltDistance, 2)) {
			Stop();
			return;
		}
	}

	m_TimeTravelled += deltaTime;

	SetPosition(ApproximateLocation());

	if (m_TimeTravelled < m_TimeToTravel) return;
	m_TimeTravelled = 0.0f;

	const auto source = GetCurrentWaypoint();

	SetPosition(source);
	m_SourcePosition = source;

	if (m_Acceleration > 0 && m_BaseSpeed > 0 && AdvanceWaypointIndex()) // Do we have another waypoint to seek?
	{
		m_NextWaypoint = GetCurrentWaypoint();
		if (m_NextWaypoint == source) {
			m_TimeToTravel = 0.0f;
		} else {
			m_CurrentSpeed = std::min(m_CurrentSpeed + m_Acceleration, m_MaxSpeed);

			const auto speed = m_CurrentSpeed * m_BaseSpeed; // scale speed based on base speed

			const auto delta = m_NextWaypoint - source;

			// Normalize the vector
			const auto length = delta.Length();
			if (length > 0.0f) {
				SetVelocity((delta / length) * speed);
			}

			// Calclute the time it will take to reach the next waypoint with the current speed
			m_TimeTravelled = 0.0f;
			m_TimeToTravel = length / speed;

			SetRotation(NiQuaternion::LookAt(source, m_NextWaypoint));
		}
	} else {
		// Check if there are more waypoints in the queue, if so set our next destination to the next waypoint
		const auto waypointNum = m_IsBounced ? m_CurrentPath.size() : m_CurrentPathWaypointCount - m_CurrentPath.size() - 1;
		RunWaypointCommands(waypointNum);
		if (m_CurrentPath.empty()) {
			if (m_Path) {
				if (m_Path->pathBehavior == PathBehavior::Loop) {
					SetPath(m_Path->pathWaypoints);
				} else if (m_Path->pathBehavior == PathBehavior::Bounce) {
					m_IsBounced = !m_IsBounced;
					std::vector<PathWaypoint> waypoints = m_Path->pathWaypoints;
					if (m_IsBounced) std::ranges::reverse(waypoints);
					SetPath(waypoints);
				} else if (m_Path->pathBehavior == PathBehavior::Once) {
					Stop();
					return;
				}
			} else {
				Stop();
				return;
			}
		} else {
			SetDestination(m_CurrentPath.top().position);

			m_CurrentPath.pop();
		}
	}

	Game::entityManager->SerializeEntity(m_Parent);
}

const MovementAIInfo& MovementAIComponent::GetInfo() const {
	return m_Info;
}

bool MovementAIComponent::AdvanceWaypointIndex() {
	if (m_PathIndex >= m_InterpolatedWaypoints.size()) {
		return false;
	}

	m_PathIndex++;

	return true;
}

NiPoint3 MovementAIComponent::GetCurrentWaypoint() const {
	return m_PathIndex >= m_InterpolatedWaypoints.size() ? m_Parent->GetPosition() : m_InterpolatedWaypoints[m_PathIndex];
}

NiPoint3 MovementAIComponent::ApproximateLocation() const {
	auto source = m_SourcePosition;

	if (AtFinalWaypoint()) return source;

	auto destination = m_NextWaypoint;

	auto percentageToWaypoint = m_TimeToTravel > 0 ? m_TimeTravelled / m_TimeToTravel : 0;

	auto approximation = source + ((destination - source) * percentageToWaypoint);

	if (dpWorld::IsLoaded()) {
		approximation.y = dpWorld::GetNavMesh()->GetHeightAtPoint(approximation);
	}

	return approximation;
}

bool MovementAIComponent::Warp(const NiPoint3& point) {
	Stop();

	NiPoint3 destination = point;

	if (dpWorld::IsLoaded()) {
		destination.y = dpWorld::GetNavMesh()->GetHeightAtPoint(point);

		if (std::abs(destination.y - point.y) > 3) {
			return false;
		}
	}

	SetPosition(destination);

	Game::entityManager->SerializeEntity(m_Parent);

	return true;
}

void MovementAIComponent::Stop() {
	if (AtFinalWaypoint()) return;

	SetPosition(ApproximateLocation());

	SetVelocity(NiPoint3Constant::ZERO);

	m_TimeToTravel = 0;
	m_TimeTravelled = 0;

	m_AtFinalWaypoint = true;

	m_InterpolatedWaypoints.clear();
	while (!m_CurrentPath.empty()) m_CurrentPath.pop();
	m_CurrentPathWaypointCount = 0;

	m_PathIndex = 0;

	m_CurrentSpeed = 0;

	Game::entityManager->SerializeEntity(m_Parent);
}

void MovementAIComponent::PullToPoint(const NiPoint3& point) {
	Stop();

	m_PullingToPoint = true;
	m_PullPoint = point;
}

void MovementAIComponent::SetPath(std::vector<PathWaypoint> path, const uint32_t waypointStart) {
	if (path.empty() || waypointStart >= path.size()) return;
	while (!m_CurrentPath.empty()) m_CurrentPath.pop();
	std::for_each(path.rbegin(), path.rend() - 1 - waypointStart, [this](const PathWaypoint& point) {
		this->m_CurrentPath.push(point);
		});

	m_CurrentPathWaypointCount = path.size();
	SetDestination(path[waypointStart].position);
}

float MovementAIComponent::GetBaseSpeed(LOT lot) {
	// Check if the lot is in the cache
	const auto& it = m_PhysicsSpeedCache.find(lot);

	if (it != m_PhysicsSpeedCache.end()) {
		return it->second;
	}

	CDComponentsRegistryTable* componentRegistryTable = CDClientManager::GetTable<CDComponentsRegistryTable>();
	CDPhysicsComponentTable* physicsComponentTable = CDClientManager::GetTable<CDPhysicsComponentTable>();

	int32_t componentID;
	CDPhysicsComponent* physicsComponent = nullptr;

	componentID = componentRegistryTable->GetByIDAndType(lot, eReplicaComponentType::CONTROLLABLE_PHYSICS, -1);

	if (componentID == -1) {
		componentID = componentRegistryTable->GetByIDAndType(lot, eReplicaComponentType::SIMPLE_PHYSICS, -1);
	}

	physicsComponent = physicsComponentTable->GetByID(componentID);

	// Client defaults speed to 10 and if the speed is also null in the table, it defaults to 10.
	float speed = physicsComponent != nullptr ? physicsComponent->speed : 10.0f;

	float delta = fabs(speed) - 1.0f;

	if (delta <= std::numeric_limits<float>::epsilon()) speed = 10.0f;

	m_PhysicsSpeedCache[lot] = speed;

	return speed;
}

void MovementAIComponent::SetPosition(const NiPoint3& value) {
	m_Parent->SetPosition(value);
}

void MovementAIComponent::SetRotation(const NiQuaternion& value) {
	if (!m_LockRotation) m_Parent->SetRotation(value);
}

NiPoint3 MovementAIComponent::GetVelocity() const {
	auto* controllablePhysicsComponent = m_Parent->GetComponent<ControllablePhysicsComponent>();

	if (controllablePhysicsComponent != nullptr) {
		return controllablePhysicsComponent->GetVelocity();
	}

	auto* simplePhysicsComponent = m_Parent->GetComponent<SimplePhysicsComponent>();

	if (simplePhysicsComponent != nullptr) {
		return simplePhysicsComponent->GetVelocity();
	}

	return NiPoint3Constant::ZERO;

}

void MovementAIComponent::SetVelocity(const NiPoint3& value) {
	auto* controllablePhysicsComponent = m_Parent->GetComponent<ControllablePhysicsComponent>();

	if (controllablePhysicsComponent != nullptr) {
		controllablePhysicsComponent->SetVelocity(value);

		return;
	}

	auto* simplePhysicsComponent = m_Parent->GetComponent<SimplePhysicsComponent>();

	if (simplePhysicsComponent != nullptr) {
		simplePhysicsComponent->SetVelocity(value);
	}
}

void MovementAIComponent::SetDestination(const NiPoint3 destination) {
	if (m_PullingToPoint) return;

	const auto location = ApproximateLocation();

	if (!AtFinalWaypoint()) {
		SetPosition(location);
	}

	m_SourcePosition = location;

	std::vector<NiPoint3> computedPath;
	if (dpWorld::IsLoaded()) {
		computedPath = dpWorld::GetNavMesh()->GetPath(m_Parent->GetPosition(), destination, m_Info.wanderSpeed);
	}

	// Somehow failed
	if (computedPath.empty()) {
		// Than take 10 points between the current position and the destination and make that the path

		auto start = location;

		auto delta = destination - start;

		auto step = delta / 10.0f;

		for (int i = 0; i < 10; i++) {
			start += step;

			computedPath.push_back(start);
		}
	}

	m_InterpolatedWaypoints.clear();

	// Simply path
	for (auto& point : computedPath) {
		if (dpWorld::IsLoaded()) {
			point.y = dpWorld::GetNavMesh()->GetHeightAtPoint(point);
		}

		m_InterpolatedWaypoints.push_back(point);
	}

	m_PathIndex = 0;

	m_TimeTravelled = 0;
	m_TimeToTravel = 0;

	m_AtFinalWaypoint = false;
}

NiPoint3 MovementAIComponent::GetDestination() const {
	return m_InterpolatedWaypoints.empty() ? m_Parent->GetPosition() : m_InterpolatedWaypoints.back();
}

void MovementAIComponent::SetMaxSpeed(const float value) {
	if (value == m_MaxSpeed) return;
	m_MaxSpeed = value;
	m_Acceleration = value / 5;
}

void MovementAIComponent::RunWaypointCommands(uint32_t waypointNum) {
	m_Parent->GetScript()->OnWaypointReached(m_Parent, waypointNum);
	m_Parent->GetScript()->OnArrived(*m_Parent, "", waypointNum, m_Path);

	if (!m_Path || waypointNum >= m_Path->pathWaypoints.size()) return;
	const auto& commands = m_Path->pathWaypoints[waypointNum].commands;
	for (const auto& [command, data] : commands) {
		LOG_DEBUG("%s %s %s", StringifiedEnum::ToString(command).data(), m_Path->pathName.c_str(), data.c_str());
		const auto dataSplit = GeneralUtils::SplitString(data, ',');
		switch (command) {
		case eWaypointCommandType::INVALID: break;
		case eWaypointCommandType::BOUNCE: break;
		case eWaypointCommandType::STOP: Pause(); break;
		case eWaypointCommandType::GROUP_EMOTE: break;
		case eWaypointCommandType::SET_VARIABLE: break; // Empty in the client
		case eWaypointCommandType::CAST_SKILL: {
			const auto skill = GeneralUtils::TryParse<uint32_t>(data);
			if (skill) {
				auto* const skillComponent = m_Parent->GetComponent<SkillComponent>();
				if (skillComponent) skillComponent->CastSkill(skill.value());
			}
			break;
		}
		case eWaypointCommandType::EQUIP_INVENTORY: {
			auto* const inventoryComponent = m_Parent->GetComponent<InventoryComponent>();
			if (inventoryComponent) {
				// items should always exist
				auto* const item = inventoryComponent->GetInventory(eInventoryType::ITEMS)->FindItemBySlot(0);
				inventoryComponent->EquipItem(item);
			}
			break;
		}
		case eWaypointCommandType::UNEQUIP_INVENTORY: {
			auto* const inventoryComponent = m_Parent->GetComponent<InventoryComponent>();
			if (inventoryComponent) {
				// items should always exist
				auto* const item = inventoryComponent->GetInventory(eInventoryType::ITEMS)->FindItemBySlot(0);
				inventoryComponent->UnEquipItem(item);
			}
			break;
		}
		case eWaypointCommandType::DELAY: {
			Pause(GeneralUtils::TryParse<float>(data).value_or(0.0f));
			break;
		}
		case eWaypointCommandType::EMOTE: {
			m_Delay = RenderComponent::GetAnimationTime(m_Parent, data);
			const auto emoteID = GeneralUtils::TryParse<uint32_t>(data);
			if (emoteID) GameMessages::SendPlayEmote(m_Parent->GetObjectID(), emoteID.value(), LWOOBJID_EMPTY, UNASSIGNED_SYSTEM_ADDRESS);
			break;
		}
		case eWaypointCommandType::TELEPORT: break;
		case eWaypointCommandType::PATH_SPEED: m_BaseSpeed = GetBaseSpeed(m_Parent->GetLOT()) * GeneralUtils::TryParse<float>(data).value_or(1.0f); break;
		case eWaypointCommandType::REMOVE_NPC: break;
		case eWaypointCommandType::CHANGE_WAYPOINT: SetPath(dataSplit[0]); break;
		case eWaypointCommandType::DELETE_SELF: break;
		case eWaypointCommandType::KILL_SELF: m_Parent->Smash(); break;
		case eWaypointCommandType::SPAWN_OBJECT: break;
		case eWaypointCommandType::PLAY_SOUND: break;
		}
	}
}
