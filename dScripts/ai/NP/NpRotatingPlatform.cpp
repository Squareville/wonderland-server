#include "NpRotatingPlatform.h"

#include "Entity.h"

void NpRotatingPlatform::OnStartup(Entity* self) {
	self->AddTimer("setvelocity", 1.0f);
}

void NpRotatingPlatform::OnTimerDone(Entity* self, const std::string timerName) {
	if (timerName == "setvelocity") {
		const auto rotation = NiPoint3(self->GetVarAs<float>(u"rotX"), self->GetVarAs<float>(u"rotY"), self->GetVarAs<float>(u"rotZ"));
		self->SetAngularVelocity(rotation);
		Game::entityManager->SerializeEntity(self);
	}
}
