#pragma once
#include "Actor.h"
#include "MeshComponent.h"
#include "AudioComponent.h"
#include "SoundEvent.h"
#include "MoveComponent.h"

class FireLoopActor : public Actor
{
public:
	FireLoopActor(class Game* game);
	void UpdateActor(float deltaTime) override;
	void ActorInput(const uint8_t* keys) override;
private:
	class MeshComponent* mc;
	class AudioComponent* ac;
	class SoundEvent se;
	class MoveComponent* moveComp;
};