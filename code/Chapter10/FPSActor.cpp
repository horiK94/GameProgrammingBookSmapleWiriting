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
#include "BallActor.h"
#include "BoxComponent.h"
#include "PlaneActor.h"
#include "FloorActor.h"
#include "PhysWorld.h"

FPSActor::FPSActor(Game* game)
	:Actor(game)
{
	mMoveComp = new MoveComponent(this);
	mAudioComp = new AudioComponent(this);
	mLastFootstep = 0.0f;
	mFootstep = mAudioComp->PlayEvent("event:/Footstep");
	mFootstep.SetPaused(true);

	mCameraComp = new FPSCamera(this);

	mFPSModel = new Actor(game);
	mFPSModel->SetScale(0.75f);
	mMeshComp = new MeshComponent(mFPSModel);
	mMeshComp->SetMesh(game->GetRenderer()->GetMesh("Assets/Rifle.gpmesh"));

	// Add a box component
	mBoxComp = new BoxComponent(this);
	AABB myBox(Vector3(-25.0f, -25.0f, -87.5f),
		Vector3(25.0f, 25.0f, 87.5f));
	mBoxComp->SetObjectBox(myBox);
	mBoxComp->SetShouldRotate(false);
}

void FPSActor::UpdateActor(float deltaTime)
{
	Actor::UpdateActor(deltaTime);

	FixCollisions();

	// Play the footstep if we're moving and haven't recently
	mLastFootstep -= deltaTime;
	if ((!Math::NearZero(mMoveComp->GetForwardSpeed()) ||
		!Math::NearZero(mMoveComp->GetStrafeSpeed())) &&
		mLastFootstep <= 0.0f)
	{
		mFootstep.SetPaused(false);
		mFootstep.Restart();
		mLastFootstep = 0.5f;
	}

	// Update position of FPS model relative to actor position
	const Vector3 modelOffset(Vector3(10.0f, 10.0f, -10.0f));
	Vector3 modelPos = GetPosition();
	modelPos += GetForward() * modelOffset.x;
	modelPos += GetRight() * modelOffset.y;
	modelPos.z += modelOffset.z;
	mFPSModel->SetPosition(modelPos);
	// Initialize rotation to actor rotation
	Quaternion q = GetRotation();
	// Rotate by pitch from camera
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
	if (keys[SDL_SCANCODE_A])
	{
		strafeSpeed -= 400.0f;
	}
	if (keys[SDL_SCANCODE_D])
	{
		strafeSpeed += 400.0f;
	}

	mMoveComp->SetForwardSpeed(forwardSpeed);
	mMoveComp->SetStrafeSpeed(strafeSpeed);

	// Mouse movement
	// Get relative movement from SDL
	int x, y;
	SDL_GetRelativeMouseState(&x, &y);
	// Assume mouse movement is usually between -500 and +500
	const int maxMouseSpeed = 500;
	// Rotation/sec at maximum speed
	const float maxAngularSpeed = Math::Pi * 8;
	float angularSpeed = 0.0f;
	if (x != 0)
	{
		// Convert to ~[-1.0, 1.0]
		angularSpeed = static_cast<float>(x) / maxMouseSpeed;
		// Multiply by rotation/sec
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

	//Jump
	if (keys[SDL_SCANCODE_SPACE])
	{
		mMoveComp->SetAccelerationSpeed(1000.0f);
		mMoveComp->SetAccelerationCalc(true);
		state = State::JUMP;
	}
}

void FPSActor::Shoot()
{
	// Get start point (in center of screen on near plane)
	Vector3 screenPoint(0.0f, 0.0f, 0.0f);
	Vector3 start = GetGame()->GetRenderer()->Unproject(screenPoint);
	// Get end point (in center of screen, between near and far)
	screenPoint.z = 0.9f;
	Vector3 end = GetGame()->GetRenderer()->Unproject(screenPoint);
	// Get direction vector
	Vector3 dir = end - start;
	dir.Normalize();
	// Spawn a ball
	BallActor* ball = new BallActor(GetGame());
	ball->SetPlayer(this);
	ball->SetPosition(start + dir * 20.0f);
	// Rotate the ball to face new direction
	ball->RotateToNewForward(dir);
	// Play shooting sound
	mAudioComp->PlayEvent("event:/Shot");
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

//Actorを移動したあとに呼ぶ関数
//コリジョン応じて、Actorの位置を微調整する
void FPSActor::FixCollisions()
{
	//// Need to recompute my world transform to update world box
	//ComputeWorldTransform();

	//const AABB& playerBox = mBoxComp->GetWorldBox();
	//Vector3 pos = GetPosition();

	//auto& planes = GetGame()->GetPlanes();
	//for (auto pa : planes)
	//{
	//	// Do we collide with this PlaneActor?
	//	const AABB& planeBox = pa->GetBox()->GetWorldBox();
	//	if (Intersect(playerBox, planeBox))
	//	{
	//		// Calculate all our differences
	//		float dx1 = planeBox.mMax.x - playerBox.mMin.x;
	//		float dx2 = planeBox.mMin.x - playerBox.mMax.x;
	//		float dy1 = planeBox.mMax.y - playerBox.mMin.y;
	//		float dy2 = planeBox.mMin.y - playerBox.mMax.y;
	//		float dz1 = planeBox.mMax.z - playerBox.mMin.z;
	//		float dz2 = planeBox.mMin.z - playerBox.mMax.z;

	//		// Set dx to whichever of dx1/dx2 have a lower abs
	//		float dx = Math::Abs(dx1) < Math::Abs(dx2) ?
	//			dx1 : dx2;
	//		// Ditto for dy
	//		float dy = Math::Abs(dy1) < Math::Abs(dy2) ?
	//			dy1 : dy2;
	//		// Ditto for dz
	//		float dz = Math::Abs(dz1) < Math::Abs(dz2) ?
	//			dz1 : dz2;
	//		
	//		// Whichever is closest, adjust x/y position
	//		if (Math::Abs(dx) <= Math::Abs(dy) && Math::Abs(dx) <= Math::Abs(dz))
	//		{
	//			pos.x += dx;
	//		}
	//		else if (Math::Abs(dy) <= Math::Abs(dx) && Math::Abs(dy) <= Math::Abs(dz))
	//		{
	//			pos.y += dy;
	//		}
	//		else
	//		{
	//			pos.z += dz;
	//		}

	//		// Need to set position and update box component
	//		SetPosition(pos);
	//		mBoxComp->OnUpdateWorldTransform();
	//	}
	//}

	//MoveComponentによって、プレイヤーの位置が変わっているため、
	//BoxComponentとActorの位置がずれている可能性がある
	//そのため、ワールド空間の更新を行い、正しいBoxComponentの位置に更新している
	ComputeWorldTransform();

	const AABB& playerBox = mBoxComp->GetWorldBox();
	Vector3 pos = GetPosition();
	auto& planes = GetGame()->GetPlanes();
	for (auto plane : planes)
	{
		//planeのバウンディングボックスを取得
		const AABB& planeBox = plane->GetBox()->GetWorldBox();		//GetWorldBox()はconst関数であるため、変数の方にもconstが必要
		if (Intersect(playerBox, planeBox))
		{
			//衝突がある場合は差を計算する
			float dx1 = planeBox.mMax.x - playerBox.mMin.x;
			float dx2 = planeBox.mMin.x - playerBox.mMax.x;
			float dx3 = planeBox.mMax.y - playerBox.mMin.y;
			float dx4 = planeBox.mMin.y - playerBox.mMax.y;
			float dx5 = planeBox.mMax.z - playerBox.mMin.z;
			float dx6 = planeBox.mMin.z - playerBox.mMax.z;

			//差の低い方をセットする
			float dx = Math::Abs(dx1) < Math::Abs(dx2) ? dx1 : dx2;
			float dy = Math::Abs(dx3) < Math::Abs(dx4) ? dx3 : dx4;
			float dz = Math::Abs(dx5) < Math::Abs(dx6) ? dx5 : dx6;

			//最も差の小さい軸で修正する
			if (Math::Abs(dx) < Math::Abs(dy)
				&& Math::Abs(dx) < Math::Abs(dz))
			{
				pos.x += dx;
			}
			else if (Math::Abs(dy) < Math::Abs(dz))
			{
				pos.y += dy;
			}
			else
			{
				pos.z += dz;
			}

			//位置を設定
			SetPosition(pos);
			//ActorのBoxComponentの位置も修正
			mBoxComp->OnUpdateWorldTransform();

			if (mMoveComp->GetAccelerationSpeed() < -5)
			{
				state = State::FALL;
			}

			if (dynamic_cast<FloorActor*>(plane))
			{
				if (state == State::FALL)
				{
					mMoveComp->SetAccelerationCalc(false);
					state = State::GROUND;
				}
			}
		}
	}

	if (state == State::GROUND)
	{
		//下向きにSegmentCastを作成し、落下してないかチェック
		const float semengLength = 110.0f;
		//start位置を自身の位置にしてしまうと、PlayerについているAABBと線分がぶつかったとして判定されてしまう
		//なぜなら、SegmentCast()はstartに近い値を返そうとするから
		//そのため、床下をstart位置にするよう変更
		Vector3 end = GetPosition();
		Vector3 dir = -1 * GetUp();
		Vector3 start = end + dir * semengLength;
		LineSegment ls(start, end);

		PhysWorld* physWorld = GetGame()->GetPhysWorld();
		PhysWorld::CollisionInfo info;

		if (physWorld->SegmentCast(ls, info))
		{
			if (dynamic_cast<FloorActor*>(info.mActor))
			{
				return;
			}
		}
		//床と設置していないため、落下処理を行う
		mMoveComp->SetAccelerationCalc(true);
		state = State::FALL;
	}
}
