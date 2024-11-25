#ifndef YRKACTOR_H
#define YRKACTOR_H

#include "CppScripts.h"

class YrkActor : public CppScripts::Script {
public:
	void OnStartup(Entity* self) override;
};

#endif  //!YRKACTOR_H
