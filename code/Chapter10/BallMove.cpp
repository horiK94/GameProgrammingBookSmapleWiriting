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



	//進行方向の線分を構築
	const float segmentLength = 30.0f;
	Vector3 start = mOwner->GetPosition();
	Vector3 dir = mOwner->GetForward();
	Vector3 end = start + dir * segmentLength;
	//線分の作成
	LineSegment ls(start, end);

	//線分とワールドの衝突判定
	PhysWorld* phys = mOwner->GetGame()->GetPhysWorld();
	PhysWorld::CollisionInfo info;
	if (phys->SegmentCast(ls, info))
	{
		//衝突があった場合
		//反射したときの方向を求める
		dir = Vector3::Reflect(dir, info.mNormal);
		mOwner->RotateToNewForward(dir);

		//ターゲットにあたったとき
		//static_cast は静的な普通の型変換を行うキャスト。static_castだとコンパイル時に評価され、キャストできない場合は、コンパイルエラーになる
		//dynamic_cast を使えば継承関係をチェックしてくれる。dynamic_castが不当なキャストのときはNULLを返す
		TargetActor* ta = dynamic_cast<TargetActor*>(info.mActor);		//actor(Actor型)を無理やりTargetActor型に変換する
		if (ta)		//nullでないとき = ぶつかったコンポーネントの所有者がTargetActorで定義している = ぶつかったコンポーネントの所持者が標的
		{
			//BallMoveを持っている唯一のActor = BallActor
			static_cast<BallActor*>(mOwner)->HitTarget();
		}
	}

	//前進速度を基準として、基底クラスで動きを更新
	MoveComponent::Update(deltaTime);
}
