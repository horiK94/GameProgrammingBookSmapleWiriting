#include "FireLoopActor.h"
#include "AudioComponent.h"
#include "SoundEvent.h"
#include "MeshComponent.h"
#include "Renderer.h"
#include "Game.h"
#include "AudioSystem.h"

FireLoopActor::FireLoopActor(Game* game)
	:Actor(game)
{
	SetPosition(Vector3(500.0f, -75.0f, 0.0f));
	SetScale(1.0f);
	mc = new MeshComponent(this);
	mc->SetMesh(GetGame()->GetRenderer()->GetMesh("Assets/Sphere.gpmesh"));

	ac = new AudioComponent(this);
	se = ac->PlayEvent("event:/FireLoop");

	moveComp = new MoveComponent(this);
}

void FireLoopActor::UpdateActor(float deltaTime)
{
	Actor::UpdateActor(deltaTime);
	// Compute new camera from this actor
	//Vector3 position = GetPosition();
	//Vector3 target = GetPosition() + GetForward() * 100.0f;
	//Vector3 up = Vector3::UnitZ;
	//Matrix4 view = Matrix4::CreateLookAt(position, target, up);
	//GetGame()->GetRenderer()->SetViewMatrix(view);
	//GetGame()->GetAudioSystem()->SetListener(view);
}

void FireLoopActor::ActorInput(const uint8_t* keys)
{
	float forwardSpeed = 0.0f;
	float angularSpeed = 0.0f;
	// wasd movement
	if (keys[SDL_SCANCODE_UP])
	{
		forwardSpeed += 1200.0f;
	}
	if (keys[SDL_SCANCODE_DOWN])
	{
		forwardSpeed -= 1200.0f;
	}
	if (keys[SDL_SCANCODE_LEFT])
	{
		angularSpeed -= Math::TwoPi;
	}
	if (keys[SDL_SCANCODE_RIGHT])
	{
		angularSpeed += Math::TwoPi;
	}

	moveComp->SetForwardSpeed(forwardSpeed);
	moveComp->SetAngularSpeed(angularSpeed);
}