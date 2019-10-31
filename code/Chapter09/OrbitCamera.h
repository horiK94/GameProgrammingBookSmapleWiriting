// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "CameraComponent.h"

class OrbitCamera : public CameraComponent
{
public:
	OrbitCamera(class Actor* owner);

	void Update(float deltaTime) override;

	float GetPitchSpeed() const { return mPitchSpeed; }
	float GetYawSpeed() const { return mYawSpeed; }

	void SetPitchSpeed(float speed) { mPitchSpeed = speed; }
	void SetYawSpeed(float speed) { mYawSpeed = speed; }
private:
	// Offset from target
	//ターゲットからのオフセット
	Vector3 mOffset;
	// Up vector of camera
	//カメラの上方ベクトル
	Vector3 mUp;
	// Rotation/sec speed of pitch
	//ピッチの角速度
	float mPitchSpeed;
	// Rotation/sec speed of yaw
	//ヨーの角速度
	float mYawSpeed;
};
