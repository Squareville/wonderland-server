#ifndef HAZMATTRUCK_H
#define HAZMATTRUCK_H

#include "CppScripts.h"

class HazmatTruck : public CppScripts::Script {
public:
	void OnStartup(Entity* self) override;
};

#endif  //!HAZMATTRUCK_H
