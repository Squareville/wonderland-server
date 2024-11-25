#include "CppScripts.h"

class YrkNpcOnTimer : public CppScripts::Script {
public:
	void OnNotifyObject(Entity* self, Entity* sender, const std::string& name, int32_t param1 = 0, int32_t param2 = 0) override;
};
