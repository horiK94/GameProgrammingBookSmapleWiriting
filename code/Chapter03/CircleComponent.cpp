// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "CircleComponent.h"
#include "Actor.h"

CircleComponent::CircleComponent(class Actor* owner)
:Component(owner)
,mRadius(0.0f)
{
	
}

const Vector2& CircleComponent::GetCenter() const
{
	return mOwner->GetPosition();
}

float CircleComponent::GetRadius() const
{
	return mOwner->GetScale() * mRadius;
}

bool Intersect(const CircleComponent& a, const CircleComponent& b)
{
	// Calculate distance squared
	//中心点どうしの距離
	Vector2 diff = a.GetCenter() - b.GetCenter();
	float distSq = diff.LengthSq();		//2乗

	// Calculate sum of radii squared
	//半径の足し算
	float radiiSq = a.GetRadius() + b.GetRadius();
	radiiSq *= radiiSq;		//2乗

	return distSq <= radiiSq;
}
