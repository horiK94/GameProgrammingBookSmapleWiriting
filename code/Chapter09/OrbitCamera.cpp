// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "OrbitCamera.h"
#include "Actor.h"

OrbitCamera::OrbitCamera(Actor* owner)
	:CameraComponent(owner)
	//,mOffset(-400.0f, 0.0f, 0.0f)
	//,mUp(Vector3::UnitZ)
	,mOffset(-400, 0, 0)
	,mUp(Vector3::UnitZ)
	,mPitchSpeed(0.0f)
	,mYawSpeed(0.0f)
{
}

void OrbitCamera::Update(float deltaTime)
{
	//CameraComponent::Update(deltaTime);
	//// Create a quaternion for yaw about world up
	//Quaternion yaw(Vector3::UnitZ, mYawSpeed * deltaTime);
	//// Transform offset and up by yaw
	//mOffset = Vector3::Transform(mOffset, yaw);
	//mUp = Vector3::Transform(mUp, yaw);

	//// Compute camera forward/right from these vectors
	//// Forward owner.position - (owner.position + offset)
	//// = -offset
	//Vector3 forward = -1.0f * mOffset;
	//forward.Normalize();
	//Vector3 right = Vector3::Cross(mUp, forward);
	//right.Normalize();
	//
	//// Create quaternion for pitch about camera right
	//Quaternion pitch(right, mPitchSpeed * deltaTime);
	//// Transform camera offset and up by pitch
	//mOffset = Vector3::Transform(mOffset, pitch);
	//mUp = Vector3::Transform(mUp, pitch);

	//// Compute transform matrix
	//Vector3 target = mOwner->GetPosition();
	//Vector3 cameraPos = target + mOffset;
	//Matrix4 view = Matrix4::CreateLookAt(cameraPos, target, mUp);
	//SetViewMatrix(view);
	
	CameraComponent::Update(deltaTime);
	//ワールド上方を軸とするヨーのクオータニオンを作成
	Quaternion yaw(Vector3::UnitZ, mYawSpeed * deltaTime);		//UnitZを軸に、yYawSpeed * deltaTime回転
	//offsetと上方ベクトルをヨーを用いて取得
	mOffset = Vector3::Transform(mOffset, yaw);
	mUp = Vector3::Transform(mUp, yaw);

	//前方ベクトルは owner.position - (owner.position + offset) = -offset より
	Vector3 forward = -1 * mOffset;
	forward.Normalize();
	//右ベクトルは
	Vector3 right = Vector3::Cross(forward, mUp);
	right.Normalize();

	Quaternion pitch(right, mPitchSpeed * deltaTime);
	//オフセットと上方をピッチで変換
	mOffset = Vector3::Transform(mOffset, pitch);
	mUp = Vector3::Transform(mUp, pitch);
	
	//変換行列を計算
	Vector3 target = mOwner->GetForward();
	Vector3 cameraPos = target + mOffset;
	Matrix4 view = Matrix4::CreateLookAt(cameraPos, target, mUp);
	SetViewMatrix(view);
}
