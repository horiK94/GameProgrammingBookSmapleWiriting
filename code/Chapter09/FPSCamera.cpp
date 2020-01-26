// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "FPSCamera.h"
#include "Actor.h"

FPSCamera::FPSCamera(Actor* owner)
	:CameraComponent(owner)
	,mPitchSpeed(0.0f)
	,mMaxPitch(Math::Pi / 3.0f)
	,mPitch(0.0f)
{
}

void FPSCamera::Update(float deltaTime)
{
	// Call parent update (doesn't do anything right now)
	//CameraComponent::Update(deltaTime);
	//// Camera position is owner position
	//Vector3 cameraPos = mOwner->GetPosition();

	//// Update pitch based on pitch speed
	//mPitch += mPitchSpeed * deltaTime;
	//// Clamp pitch to [-max, +max]
	//mPitch = Math::Clamp(mPitch, -mMaxPitch, mMaxPitch);
	//// Make a quaternion representing pitch rotation,
	//// which is about owner's right vector
	//Quaternion q(mOwner->GetRight(), mPitch);
	//
	//// Rotate owner forward by pitch quaternion
	//Vector3 viewForward = Vector3::Transform(
	//	mOwner->GetForward(), q);
	//// Target position 100 units in front of view forward
	//Vector3 target = cameraPos + viewForward * 100.0f;
	//// Also rotate up by pitch quaternion
	//Vector3 up = Vector3::Transform(Vector3::UnitZ, q);
	//
	//// Create look at matrix, set as view
	//Matrix4 view = Matrix4::CreateLookAt(cameraPos, target, up);
	//SetViewMatrix(view);
	
	CameraComponent::Update(deltaTime);
	//カメラの位置は所有アクターの位置
	Vector3 cameraPos = mOwner->GetPosition();

	mPitch += mPitchSpeed * deltaTime;
	mPitch = Math::Clamp(mPitch, -mMaxPitch, mMaxPitch);
	//右方向ベクトルを軸に、mPitchだけ回転した回転Quaternionを取得
	Quaternion q(mOwner->GetRight(), mPitch);

	//ピッチのクオータニオンで、所有アクターの前方ベクトルを回転させる
	Vector3 viewForward = Vector3::Transform(mOwner->GetForward(), q);
	//ピットの移動を最大値以内に抑える
	//ターゲット位置は所有アクターの前方100
	Vector3 target = cameraPos + /*mOwner->GetForward()*/viewForward * 100;
	//上方ベクトルは常にz軸の基本ベクトル
	Vector3 up = Vector3::UnitZ;
	//注視行列を作成し、ビューに設定
	Matrix4 view = Matrix4::CreateLookAt(cameraPos, target, up);
	SetViewMatrix(view);
}
