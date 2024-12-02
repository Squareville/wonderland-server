#ifndef BUBBLESTATUE_H
#define BUBBLESTATUE_H

#include "CppScripts.h"

class BubbleStatue : public CppScripts::Script {
public:
	void OnStartup(Entity* self) override;
	void OnProximityUpdate(Entity* self, Entity* entering, std::string name, std::string status) override;
	void OnNotifyObject(Entity* self, Entity* sender, const std::string& name, int32_t param1 = 0, int32_t param2 = 0) override;
};

#endif  //!BUBBLESTATUE_H
