// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Laser.h"
#include "SpriteComponent.h"
#include "Game.h"
#include "CircleComponent.h"
#include "Asteroid.h"

Laser::Laser(Game* game)
	:Actor(game)
	,mDeathTimer(1.0f)
{
	// Create a sprite component
	SpriteComponent* sc = new SpriteComponent(this);
	sc->SetTexture(game->GetTexture("Assets/Laser.png"));

	// Create a move component, and set a forward speed
	mc = new MoveComponent(this, 1.0f);
	//�O�ɒ��i���邾���Ȃ̂ł����ok

	mCircle = new CircleComponent(this);
	mCircle->SetRadius(11.0f);
}

void Laser::UpdateActor(float deltaTime)
{
	mc->AddForce(Vector2::UnitX * 1500.0f);

	//�f���Ƃ̓����蔻��`�F�b�N
	// If we run out of time, laser is dead
	mDeathTimer -= deltaTime;
	if (mDeathTimer <= 0.0f)
	{
		//���ȏ�̕\�������������
		SetState(EDead);
	}
	else
	{
		// Do we intersect with an asteroid?
		for (auto ast : GetGame()->GetAsteroids())
		{
			if (Intersect(*mCircle, *(ast->GetCircle())))
			{
				//�f���ƃ��[�U�[�����������ꍇ�͂ǂ���������
				// The first asteroid we intersect with,
				// set ourselves and the asteroid to dead
				SetState(EDead);
				ast->SetState(EDead);
				break;
			}
		}
	}
}
