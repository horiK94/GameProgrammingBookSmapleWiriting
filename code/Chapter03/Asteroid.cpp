// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Asteroid.h"
#include "SpriteComponent.h"
#include "MoveComponent.h"
#include "Game.h"
#include "Random.h"
#include "CircleComponent.h"

Asteroid::Asteroid(Game* game)
	:Actor(game)
	,mCircle(nullptr)
{
	// Initialize to random position/orientation
	Vector2 randPos = Random::GetVector(Vector2::Zero,
		Vector2(1024.0f, 768.0f));
	SetPosition(randPos);

	SetRotation(Random::GetFloatRange(0.0f, Math::TwoPi));

	// Create a sprite component
	//�摜�R���|�[�l���g�̃A�^�b�`
	SpriteComponent* sc = new SpriteComponent(this);
	sc->SetTexture(game->GetTexture("Assets/Asteroid.png"));

	// Create a move component, and set a forward speed
	//�ړ��R���|�[�l���g�̃A�^�b�`
	mc = new MoveComponent(this, 100.0f);

	// Create a circle component (for collision)
	//�~�R���C�_�[�R���|�[�l���g�̃A�^�b�`
	mCircle = new CircleComponent(this);
	mCircle->SetRadius(40.0f);

	// Add to mAsteroids in game
	//���[�U�[�Ɠ����蔻������邽�߂ɁA�S�f���̓o�^
	game->AddAsteroid(this);
}

Asteroid::~Asteroid()
{
	GetGame()->RemoveAsteroid(this);
}

void Asteroid::UpdateActor(float delatTime)
{
	mc->AddForce(Vector2::UnitX * 200);
}
