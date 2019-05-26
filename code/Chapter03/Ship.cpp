// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Ship.h"
#include "SpriteComponent.h"
#include "InputComponent.h"
#include "CircleComponent.h"
#include "Game.h"
#include "Laser.h"
#include "Asteroid.h"

Ship::Ship(Game* game)
	:Actor(game)
	,mLaserCooldown(0.0f)
{
	// Create a sprite component
	SpriteComponent* sc = new SpriteComponent(this, 150);		//150に指定することによって、レーザーが前に描画されるようにしている
	sc->SetTexture(game->GetTexture("Assets/Ship.png"));

	//宇宙船と小惑星との衝突用コンポーネントのアタッチ
	cc = new CircleComponent(this);
	cc->SetRadius(20.0f);

	// Create an input component and set keys/speed
	InputComponent* ic = new InputComponent(this);
	ic->SetForwardKey(SDL_SCANCODE_W);
	ic->SetBackKey(SDL_SCANCODE_S);
	ic->SetClockwiseKey(SDL_SCANCODE_A);
	ic->SetCounterClockwiseKey(SDL_SCANCODE_D);
	ic->SetMaxForwardSpeed(300.0f);
	ic->SetMaxAngularSpeed(Math::TwoPi);
}

void Ship::UpdateActor(float deltaTime)
{
	//レーザーのクールダウン時間
	mLaserCooldown -= deltaTime;
	deathRecoveryTime -= deltaTime;
	CheckIntersectAsteroid();
	RecoveryMyself();
	//SDL_Log("result : %f", deathRecoveryTime);
}

void Ship::ActorInput(const uint8_t* keyState)
{
	//入力に関するoverride関数
	if (keyState[SDL_SCANCODE_SPACE] && mLaserCooldown <= 0.0f)
	{
		// Create a laser and set its position/rotation to mine
		//レーザーのインスタンス生成(Actorとして　)
		Laser* laser = new Laser(GetGame());
		laser->SetPosition(GetPosition());
		laser->SetRotation(GetRotation());

		// Reset laser cooldown (half second)
		mLaserCooldown = 0.5f;
	}
}

void Ship::CheckIntersectAsteroid()
{
	if (GetState() == State::EHidden)
	{
		//衝突判定を行わない
		return;
	}
	for (Asteroid* ob : GetGame()->GetAsteroids())
	{
		//惑星と衝突したとき
		if (Intersect(*cc, *(ob->GetCircle())))
		{
			ob->SetState(State::EDead);
			SetState(State::EHidden);
			deathRecoveryTime = 1.5f;		//復帰にかかる時間
		}
	}
}

void Ship::ResetPosAndRotate()
{
	this->SetPosition(Vector2(512.0f, 384.0f));
	this->SetRotation(0.0f);
}

void Ship::RecoveryMyself()
{
	if (this->GetState() != State::EHidden)
	{
		return;		//復帰しないため終了
	}
	if (deathRecoveryTime <= 0)
	{
		//復帰
		ResetPosAndRotate();
		SetState(State::EActive);
	}
}