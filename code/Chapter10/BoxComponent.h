// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "Component.h"
#include "Collision.h"

class BoxComponent : public Component
{
public:
	BoxComponent(class Actor* owner, int updateOrder = 100);
	~BoxComponent();

	void OnUpdateWorldTransform() override;		//Updateではない。所有アクターがワールド変換を再計算するときに呼ばれる

	void SetObjectBox(const AABB& model) { mObjectBox = model; }
	const AABB& GetWorldBox() const { return mWorldBox; }

	void SetShouldRotate(bool value) { mShouldRotate = value; }		//アクターが回転するときにBoxComponentも回転させるか設定
private:
	AABB mObjectBox;		//オブジェクト空間のバウンディングボックス
	AABB mWorldBox;			//ワールド空間のバウンディングボックス.所有アクターのワールド変換に追従
	bool mShouldRotate;
};
