// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "MoveComponent.h"
#include "Actor.h"

MoveComponent::MoveComponent(class Actor* owner, float mass, int updateOrder)
	:Component(owner, updateOrder)
	, mAngularSpeed(0.0f)
	, mass(mass)
	, sumForce(Vector2::Zero)
	, velocity(Vector2::Zero)
{
	if (mass <= 0)
	{
		mass = 1.0f;
	}
}

void MoveComponent::Update(float deltaTime)
{
	if (!Math::NearZero(mAngularSpeed))
	{
		float rot = mOwner->GetRotation();
		rot += mAngularSpeed * deltaTime;
		mOwner->SetRotation(rot);
	}

	//加速度はsumForce = 0のとき0になるため、force = Vector2::Zeroでも物理計算を行わなければならない
	float angle = -1 * mOwner->GetRotation();
	Vector2 forceConsiderAngle = Vector2(cos(angle) * sumForce.x - sin(angle) * sumForce.y, sin(angle) * sumForce.x + cos(angle) * sumForce.y);
	//加速度を求める
	Vector2 acceleration = forceConsiderAngle / mass;
	//加速度から速度を求める
	velocity += acceleration * deltaTime;
	//位置を更新
	Vector2 pos = mOwner->GetPosition() + velocity * deltaTime;

	// (Screen wrapping code only for asteroids)
	if (pos.x < 0.0f) { pos.x = 1022.0f; }
	else if (pos.x > 1024.0f) { pos.x = 2.0f; }

	if (pos.y < 0.0f) { pos.y = 766.0f; }
	else if (pos.y > 768.0f) { pos.y = 2.0f; }

	mOwner->SetPosition(pos);

	sumForce = Vector2::Zero;
}