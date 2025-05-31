#include "NpRotatingPlatform.h"

#include "Entity.h"

// function onStartup(self)
//     GAMEOBJ:GetTimer():AddTimerWithCancel( 1, "setvelocity", self )
//     self:SetUpdatable{bUpdatable = true}
// end

// function onTimerDone(self, msg)
//     if "setvelocity" == msg.name then
//         local rotation = {x = self:GetVar("rotX"), y = self:GetVar("rotY"), z = self:GetVar("rotZ")}
//         self:SetAngularVelocity{angVelocity = rotation, bIgnoreDirtyFlags = false}
//     end
// end


void NpRotatingPlatform::OnStartup(Entity* self) {
	self->AddTimer("setvelocity", 1.0f);
}
void NpRotatingPlatform::OnTimerDone(Entity* self, const std::string timerName) {
	if (timerName == "setvelocity") {
		auto rotation = NiPoint3(self->GetVar<float>(u"rotX"), self->GetVar<float>(u"rotY"), self->GetVar<float>(u"rotZ"));
		// angularvelocity and bIgnoreDirtyFlags
		// DLU has no concept of angular velocity
		// self->SetAngularVelocity(rotation, false);
	}
}