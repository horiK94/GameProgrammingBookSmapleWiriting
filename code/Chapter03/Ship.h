// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "Actor.h"
#include "CircleComponent.h"
#include "InputComponent.h"
class Ship : public Actor
{
public:
	Ship(class Game* game);

	void UpdateActor(float deltaTime) override;
	void ActorInput(const uint8_t* keyState) override;
private:
	float mLaserCooldown;
	float deathRecoveryTime;
	CircleComponent* cc;
	InputComponent* ic;

	void CheckIntersectAsteroid();
	void ResetPosAndRotate();
	void RecoveryMyself();
};