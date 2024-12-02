#ifndef HAZMATMISSIONGIVER_H
#define HAZMATMISSIONGIVER_H

#include "CppScripts.h"

class HazmatMissionGiver : public CppScripts::Script {
public:
	void OnStartup(Entity* self) override;

};

#endif  //!HAZMATMISSIONGIVER_H
