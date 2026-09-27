#ifndef NPMETEORCUTSCENETRIGGER_H
#define NPMETEORCUTSCENETRIGGER_H

#include "CppScripts.h"

class NpMeteorCutsceneTrigger : public CppScripts::Script {
public:
	void OnCollisionPhantom(Entity* self, Entity* target) override;
};

#endif //!NPMETEORCUTSCENETRIGGER_H
