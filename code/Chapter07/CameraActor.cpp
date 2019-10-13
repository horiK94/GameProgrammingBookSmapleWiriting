// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "CameraActor.h"
#include "MoveComponent.h"
#include "SDL/SDL_scancode.h"
#include "Renderer.h"
#include "AudioSystem.h"
#include "Game.h"
#include "AudioComponent.h"
#include "MeshComponent.h"

CameraActor::CameraActor(Game* game)
	:Actor(game)
	,mCameraPos(Vector3::Zero)
{
	mMoveComp = new MoveComponent(this);
	mAudioComp = new AudioComponent(this);
	MeshComponent* mc = new MeshComponent(this);
	mc->SetMesh(game->GetRenderer()->GetMesh("Assets/Sphere.gpmesh"));
	mLastFootstep = 0.0f;
	//mFootstep = mAudioComp->PlayEvent("event:/Footstep");
	//mFootstep.SetPaused(true);
	mCameraPos = GetPosition() - GetForward() * 200.0f + Vector3::UnitZ * 100.0f;
	SDL_Log("CameraActor");
}

void CameraActor::UpdateActor(float deltaTime)
{
	Actor::UpdateActor(deltaTime);

	// Play the footstep if we're moving and haven't recently
	mLastFootstep -= deltaTime;
	if (!Math::NearZero(mMoveComp->GetForwardSpeed()) && mLastFootstep <= 0.0f)
	{
		mFootstep.SetPaused(false);
		mFootstep.Restart();
		mLastFootstep = 0.5f;
	}

	// Compute new camera from this actor
	mCameraPos = GetPosition() - GetForward() * 200.0f + Vector3::UnitZ * 100.0f;
	Vector3 target = GetPosition() + GetForward() * 100.0f;
	Vector3 up = Vector3::UnitZ;
	Matrix4 view = Matrix4::CreateLookAt(mCameraPos, target, up);
	GetGame()->GetRenderer()->SetViewMatrix(view);
	GetGame()->GetAudioSystem()->SetListener(view);
}

void CameraActor::ActorInput(const uint8_t* keys)
{
	float forwardSpeed = 0.0f;
	float angularSpeed = 0.0f;
	// wasd movement
	if (keys[SDL_SCANCODE_W])
	{
		forwardSpeed += 300.0f;
	}
	if (keys[SDL_SCANCODE_S])
	{
		forwardSpeed -= 300.0f;
	}
	if (keys[SDL_SCANCODE_A])
	{
		angularSpeed -= Math::TwoPi;
	}
	if (keys[SDL_SCANCODE_D])
	{
		angularSpeed += Math::TwoPi;
	}

	mMoveComp->SetForwardSpeed(forwardSpeed);
	mMoveComp->SetAngularSpeed(angularSpeed);
}

void CameraActor::SetFootstepSurface(float value)
{
	// Pause here because the way I setup the parameter in FMOD
	// changing it will play a footstep
	mFootstep.SetPaused(true);
	mFootstep.SetParameter("Surface", value);
}
