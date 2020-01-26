// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "CameraComponent.h"

class FollowCamera : public CameraComponent
{
public:
	FollowCamera(class Actor* owner);

	void Update(float deltaTime) override;
	
	void SnapToIdeal();

	void SetHorzDist(float dist) { mHorzDist = dist; }
	void SetVertDist(float dist) { mVertDist = dist; }
	void SetTargetDist(float dist) { mTargetDist = dist; }
	void SetSpringConstant(float spring) { mSpringConstant = spring; }
private:
	Vector3 ComputeCameraPos() const;

	//実際のカメラの位置
	// Actual position of camera
	Vector3 mActualPos;
	// Velocity of actual camera
	//カメラの速度
	Vector3 mVelocity;
	// Horizontal follow distance
	//水平距離
	float mHorzDist;
	// Vertical follow distance
	//垂直距離
	float mVertDist;
	// Target distance
	//ターゲット距離
	float mTargetDist;
	//ばね定数(大きいほど硬い)
	// Spring constant (higher is more stiff)
	float mSpringConstant;
};
