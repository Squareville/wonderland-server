#ifndef __SKUNKEVENT__H__
#define __SKUNKEVENT__H__

#include "CppScripts.h"

class Entity;

enum class SkunkEventState : uint32_t;

class SkunkEvent : public CppScripts::Script {
public:
	void OnStartup(Entity* self) override;
	void OnPlayerLoaded(Entity* self, Entity* player) override;
private:
	void InitZoneVars(Entity* self);
	void SetZoneState(Entity* self, SkunkEventState state);
	SkunkEventState GetZoneState(Entity* self);
	const std::u16string m_ZoneState = u"ZoneState";
};

#endif  //!__SKUNKEVENT__H__
