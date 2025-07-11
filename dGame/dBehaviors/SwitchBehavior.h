#pragma once
#include "Behavior.h"

class SwitchBehavior final : public Behavior
{
public:
	Behavior* m_actionTrue;

	Behavior* m_actionFalse;

	uint32_t m_imagination;
	uint32_t m_armor;
	uint32_t m_health;

	bool m_isEnemyFaction;

	int32_t m_targetHasBuff;

	float m_Distance;

	/*
	 * Inherited
	 */

	explicit SwitchBehavior(const uint32_t behaviorId) : Behavior(behaviorId) {
	}

	void Handle(BehaviorContext* context, RakNet::BitStream& bitStream, BehaviorBranchContext branch) override;

	void Calculate(BehaviorContext* context, RakNet::BitStream& bitStream, BehaviorBranchContext branch) override;

	void Load() override;
};
