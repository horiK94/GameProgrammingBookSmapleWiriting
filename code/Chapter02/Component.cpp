// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Component.h"
#include "Actor.h"

Component::Component(Actor* owner, int updateOrder)
	:mOwner(owner)
	,mUpdateOrder(updateOrder)
{
	// Add to actor's vector of components
	mOwner->AddComponent(this);			//ownerのcomponentに追加する
}

Component::~Component()
{
	mOwner->RemoveComponent(this);		//ownerのcomponentのリストから外れる
}

void Component::Update(float deltaTime)
{
}
