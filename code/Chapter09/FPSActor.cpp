// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "FPSActor.h"
#include "MoveComponent.h"
#include "SDL/SDL_scancode.h"
#include "Renderer.h"
#include "AudioSystem.h"
#include "Game.h"
#include "AudioComponent.h"
#include "FPSCamera.h"
#include "MeshComponent.h"

FPSActor::FPSActor(Game* game)
	:Actor(game)
{
	mMoveComp = new MoveComponent(this);		//�O��̈ړ��͂���őΉ�
	mAudioComp = new AudioComponent(this);
	mLastFootstep = 0.0f;
	mFootstep = mAudioComp->PlayEvent("event:/Footstep");
	mFootstep.SetPaused(true);

	mCameraComp = new FPSCamera(this);

	//FPSModel�A�N�^�[�̍쐬
	mFPSModel = new Actor(game);
	mFPSModel->SetScale(0.75f);
	mMeshComp = new MeshComponent(mFPSModel);
	mMeshComp->SetMesh(game->GetRenderer()->GetMesh("Assets/Rifle.gpmesh"));
}

void FPSActor::UpdateActor(float deltaTime)
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
	
	// Update position of FPS model relative to actor position
	//FPS���f�����A�A�N�^�[�ɑ΂��Ăǂ̂悤�ȑ��Έʒu������
	const Vector3 modelOffset(Vector3(10.0f, 10.0f, -10.0f));
	Vector3 modelPos = GetPosition();
	modelPos += GetForward() * modelOffset.x;			//�O������10�ړ�
	modelPos += GetRight() * modelOffset.y;				//�E������10�ړ�
	modelPos.z += modelOffset.z;		//modelPos.z += Vector3::UnitZ(= (0, 0, 1)) * modelOffset.z;
	mFPSModel->SetPosition(modelPos);

	// Initialize rotation to actor rotation
	Quaternion q = GetRotation();		//���E�̉�]�擾
	// Rotate by pitch from camera
	//�㉺�̉�]�̓J�����̉�]�ł��邩��A�J�����̃s�b�`�ɂ���]��q����]������
	q = Quaternion::Concatenate(q, Quaternion(GetRight(), mCameraComp->GetPitch()));
	mFPSModel->SetRotation(q);
}

void FPSActor::ActorInput(const uint8_t* keys)
{
	float forwardSpeed = 0.0f;
	float strafeSpeed = 0.0f;
	// wasd movement
	if (keys[SDL_SCANCODE_W])
	{
		forwardSpeed += 400.0f;
	}
	if (keys[SDL_SCANCODE_S])
	{
		forwardSpeed -= 400.0f;
	}
	//if (keys[SDL_SCANCODE_A])
	//{
	//	strafeSpeed -= 400.0f;
	//}
	//if (keys[SDL_SCANCODE_D])
	//{
	//	strafeSpeed += 400.0f;
	//}
	if (keys[SDL_SCANCODE_D])
	{
		strafeSpeed += 400.0f;
	}
	if (keys[SDL_SCANCODE_A])		//else if �ɂ���ƁAD��A������������0�ɂȂ�Ȃ�
	{
		strafeSpeed -= 400.0f;
	}

	mMoveComp->SetForwardSpeed(forwardSpeed);		//�㉺�ړ�
	mMoveComp->SetStrafeSpeed(strafeSpeed);		//���E�ړ�

	// Mouse movement
	// Get relative movement from SDL
	//int x, y;
	//SDL_GetRelativeMouseState(&x, &y);
	//// Assume mouse movement is usually between -500 and +500
	//const int maxMouseSpeed = 500;
	//// Rotation/sec at maximum speed
	//const float maxAngularSpeed = Math::Pi * 8;
	//float angularSpeed = 0.0f;
	//if (x != 0)
	//{
	//	// Convert to ~[-1.0, 1.0]
	//	angularSpeed = static_cast<float>(x) / maxMouseSpeed;
	//	// Multiply by rotation/sec
	//	angularSpeed *= maxAngularSpeed;
	//}
	//mMoveComp->SetAngularSpeed(angularSpeed);
	
	//�}�E�X�ړ�
	int x, y;
	Uint32 buttons = SDL_GetRelativeMouseState(&x, &y);
	//�}�E�X�̓�����-500 �` 500�͈̔͂Ƃ���
	const int maxMouseSpeed = 500;
	//�ő�ړ���(500)�̂Ƃ��̊p���x�ݒ�
	const float maxAngularSpeed = 8 * Math::Pi;
	float angularSpeed = 0;

	if (x != 0)
	{
		angularSpeed = static_cast<float>(x) / maxMouseSpeed;
		//�ő�ړ��ʂ̂Ƃ��̊p���x��������
		angularSpeed *= maxAngularSpeed;
	}
	mMoveComp->SetAngularSpeed(angularSpeed);

	// Compute pitch
	const float maxPitchSpeed = Math::Pi * 8;
	float pitchSpeed = 0.0f;
	if (y != 0)
	{
		// Convert to ~[-1.0, 1.0]
		pitchSpeed = static_cast<float>(y) / maxMouseSpeed;
		pitchSpeed *= maxPitchSpeed;
	}
	mCameraComp->SetPitchSpeed(pitchSpeed);
}

void FPSActor::SetFootstepSurface(float value)
{
	// Pause here because the way I setup the parameter in FMOD
	// changing it will play a footstep
	mFootstep.SetPaused(true);
	mFootstep.SetParameter("Surface", value);
}

void FPSActor::SetVisible(bool visible)
{
	mMeshComp->SetVisible(visible);
}
