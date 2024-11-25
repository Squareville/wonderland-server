#ifndef BABYSKUNKS_H
#define BABYSKUNKS_H

#include "CppScripts.h"

class BabySkunks : public CppScripts::Script {
public:
	void OnStartup(Entity* self) override;
	void OnProximityUpdate(Entity* self, Entity* entering, std::string name, std::string status) override;
};

#endif  //!BABYSKUNKS_H
