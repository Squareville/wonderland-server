#ifndef NPMONUMENTQUICKBUILD_H
#define NPMONUMENTQUICKBUILD_H

#include "CppScripts.h"

class NpMonumentQuickbuild : public CppScripts::Script {
public:
	void OnQuickBuildComplete(Entity* self, Entity* target) override;
};

#endif //!NPMONUMENTQUICKBUILD_H
