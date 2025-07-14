// Darkflame Universe
// Copyright 2025

#ifndef PICNICBOTPICNICBOT_H
#define PICNICBOTPICNICBOT_H

#include "CppScripts.h"

class PicnicBotPicnicBot : public CppScripts::Script {
public:
	void OnArrived(Entity& self, const std::string& pathType, const uint32_t waypoint, const Path* const levelPath) override;
};

#endif //!PICNICBOTPICNICBOT_H
