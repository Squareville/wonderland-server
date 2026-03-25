#include "AfvDarklingRegenerator.h"

#include "SkillComponent.h"

void AfvDarklingRegenerator::OnStartup(Entity* self) {
	//start a timer, randomized from 3 to 5 seconds
	self->AddTimer("AfvDarklingRegeneratorTimer", GeneralUtils::GenerateRandomNumber<float>(3.0, 5.0));
}

void AfvDarklingRegenerator::OnTimerDone(Entity* self, std::string timerName) {
	if (timerName == "AfvDarklingRegeneratorTimer") {
		auto* skillComponent = self->GetComponent<SkillComponent>();
		if (skillComponent == nullptr) return;
		// cast skill 2209 and restart the timer
		skillComponent->CastSkill(2209, LWOOBJID_EMPTY, self->GetObjectID());
		self->AddTimer("AfvDarklingRegeneratorTimer", GeneralUtils::GenerateRandomNumber<float>(3.0, 5.0));
	}
}
