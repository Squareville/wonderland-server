#include "NpRotatingPlatform.h"

#include "Entity.h"

void NpRotatingPlatform::OnStartup(Entity* self) {
	self->AddTimer("setvelocity", 1.0f);
}

void NpRotatingPlatform::OnTimerDone(Entity* self, const std::string timerName) {
	if (timerName == "setvelocity") {
		const auto rotation = NiPoint3(self->GetVar<float>(u"rotX"), self->GetVar<float>(u"rotY"), self->GetVar<float>(u"rotZ"));
		self->SetAngularVelocity(rotation);
	}
}
