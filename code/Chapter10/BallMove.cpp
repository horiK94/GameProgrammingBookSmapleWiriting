// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "BallMove.h"
#include "Actor.h"
#include "Game.h"
#include "PhysWorld.h"
#include "TargetActor.h"
#include "BallActor.h"

BallMove::BallMove(Actor* owner)
	:MoveComponent(owner)
{
}

void BallMove::Update(float deltaTime)
{
	// Construct segment in direction of travel
	//const float segmentLength = 30.0f;
	//Vector3 start = mOwner->GetPosition();
	//Vector3 dir = mOwner->GetForward();
	//Vector3 end = start + dir * segmentLength;

	//// Create line segment
	//LineSegment l(start, end);

	//// Test segment vs world
	//PhysWorld* phys = mOwner->GetGame()->GetPhysWorld();
	//PhysWorld::CollisionInfo info;
	//// (Don't collide vs player)
	//if (phys->SegmentCast(l, info) && info.mActor != mPlayer)
	//{
	//	// If we collided, reflect the ball about the normal
	//	dir = Vector3::Reflect(dir, info.mNormal);
	//	mOwner->RotateToNewForward(dir);
	//	// Did we hit a target?
	//	TargetActor* target = dynamic_cast<TargetActor*>(info.mActor);
	//	if (target)
	//	{
	//		static_cast<BallActor*>(mOwner)->HitTarget();
	//	}
	//}

	// Base class update moves based on forward speed
	//MoveComponent::Update(deltaTime);



	//�i�s�����̐������\�z
	const float segmentLength = 30.0f;
	Vector3 start = mOwner->GetPosition();
	Vector3 dir = mOwner->GetForward();
	Vector3 end = start + dir * segmentLength;
	//�����̍쐬
	LineSegment ls(start, end);

	//�����ƃ��[���h�̏Փ˔���
	PhysWorld* phys = mOwner->GetGame()->GetPhysWorld();
	PhysWorld::CollisionInfo info;
	if (phys->SegmentCast(ls, info))
	{
		//�Փ˂��������ꍇ
		//���˂����Ƃ��̕��������߂�
		dir = Vector3::Reflect(dir, info.mNormal);
		mOwner->RotateToNewForward(dir);

		//�^�[�Q�b�g�ɂ��������Ƃ�
		//static_cast �͐ÓI�ȕ��ʂ̌^�ϊ����s���L���X�g�Bstatic_cast���ƃR���p�C�����ɕ]������A�L���X�g�ł��Ȃ��ꍇ�́A�R���p�C���G���[�ɂȂ�
		//dynamic_cast ���g���Όp���֌W���`�F�b�N���Ă����Bdynamic_cast���s���ȃL���X�g�̂Ƃ���NULL��Ԃ�
		TargetActor* ta = dynamic_cast<TargetActor*>(info.mActor);		//actor(Actor�^)�𖳗����TargetActor�^�ɕϊ�����
		if (ta)		//null�łȂ��Ƃ� = �Ԃ������R���|�[�l���g�̏��L�҂�TargetActor�Œ�`���Ă��� = �Ԃ������R���|�[�l���g�̏����҂��W�I
		{
			//BallMove�������Ă���B���Actor = BallActor
			static_cast<BallActor*>(mOwner)->HitTarget();
		}
	}

	//�O�i���x����Ƃ��āA���N���X�œ������X�V
	MoveComponent::Update(deltaTime);
}
