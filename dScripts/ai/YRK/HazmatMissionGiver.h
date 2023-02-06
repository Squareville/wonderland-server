#ifndef __HAZMATMISSIONGIVER__H__
#define __HAZMATMISSIONGIVER__H__

#include "CppScripts.h"

class HazmatMissionGiver : public CppScripts::Script {
public:
	void OnStartup(Entity* self) override;
};

#endif  //!__HAZMATMISSIONGIVER__H__
