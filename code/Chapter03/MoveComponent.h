// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "SDL/SDL.h"
#include "Component.h"
#include "Math.h"

class MoveComponent : public Component
{
public:
	// Lower update order to update first
	MoveComponent(class Actor* owner, float mass, int updateOrder = 10);

	void Update(float deltaTime) override;
	
	float GetAngularSpeed() const { return mAngularSpeed; }
	void SetAngularSpeed(float speed) { mAngularSpeed = speed; }
	void AddForce(Vector2 force) { sumForce += force; }
	void ResetVelocity() { velocity = Vector2::Zero; }
private:
	// Controls rotation (radians/second)
	float mAngularSpeed;
	//フレーム内で加算された力の合計
	Vector2 sumForce;
	//速度
	Vector2 velocity;
	//質量
	float mass;
};
