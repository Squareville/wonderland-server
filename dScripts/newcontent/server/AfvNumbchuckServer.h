#ifndef AFVNUMBCHUCKSERVER_H
#define AFVNUMBCHUCKSERVER_H

#include "CppScripts.h"

class AfvNumbchuckServer : public CppScripts::Script {
public:
	void OnStartup(Entity* self) override;
	void OnProximityUpdate(Entity* self, Entity* entering, std::string name, std::string status) override;
};

#endif //!AFVNUMBCHUCKSERVER_H
