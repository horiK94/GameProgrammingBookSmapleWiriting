// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "BoxComponent.h"
#include "Actor.h"
#include "Game.h"
#include "PhysWorld.h"

BoxComponent::BoxComponent(Actor* owner, int updateOrder)
	:Component(owner, updateOrder)
	,mObjectBox(Vector3::Zero, Vector3::Zero)
	,mWorldBox(Vector3::Zero, Vector3::Zero)
	,mShouldRotate(true)
{
	//mOwner->GetGame()->GetPhysWorld()->AddBox(this);
	mOwner->GetGame()->GetPhysWorld()->AddBox(this);		//このコンポーネントを、BoxComponentを管理しているPhysWorldにAdd
}

BoxComponent::~BoxComponent()
{
	//mOwner->GetGame()->GetPhysWorld()->RemoveBox(this);
	mOwner->GetGame()->GetPhysWorld()->RemoveBox(this);
}

void BoxComponent::OnUpdateWorldTransform()
{
	//// Reset to object space box
	//mWorldBox = mObjectBox;
	//// Scale
	//mWorldBox.mMin *= mOwner->GetScale();
	//mWorldBox.mMax *= mOwner->GetScale();
	//// Rotate (if we want to)
	//if (mShouldRotate)
	//{
	//	mWorldBox.Rotate(mOwner->GetRotation());
	//}
	//// Translate
	//mWorldBox.mMin += mOwner->GetPosition();
	//mWorldBox.mMax += mOwner->GetPosition();

	//オブジェクト空間をスケーリング、回転、平行移動を適用し、ワールド座標にする
	mWorldBox = mObjectBox;
	//スケーリング適用
	mWorldBox.mMin *= mOwner->GetScale();
	mWorldBox.mMax *= mOwner->GetScale();

	//回転適用
	if (mShouldRotate)
	{
		//回転をする設定なら所有アクターのQuaternionを渡す
		mWorldBox.Rotate(mOwner->GetRotation());
	}
	//平行移動
	mWorldBox.mMin += mOwner->GetPosition();
	mWorldBox.mMax += mOwner->GetPosition();
}
