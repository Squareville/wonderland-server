#ifndef NP_MIME_H
#define NP_MIME_H
#include "CppScripts.h"
#include "Entity.h"

class NpMime : public CppScripts::Script {
public:
	void OnWaypointReached(Entity* self, uint32_t waypointIndex) override;
	void OnTimerDone(Entity* self, std::string timerName) override;
private:
	NiPoint3 m_JugglePosition = NiPoint3(-707.57f, 233.52f, 486.55f);
	NiQuaternion m_JuggleRotation = NiQuaternion(0.6908824443817139f, 0.0f, 0.7229671478271484f, 0.0f);
};

#endif // NP_MIME_H
