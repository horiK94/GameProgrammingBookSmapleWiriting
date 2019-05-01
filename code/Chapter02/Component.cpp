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
	mOwner->AddComponent(this);			//owner‚Ìcomponent‚É’Ç‰Á‚·‚é
}

Component::~Component()
{
	mOwner->RemoveComponent(this);		//owner‚Ìcomponent‚ÌƒŠƒXƒg‚©‚çŠO‚ê‚é
}

void Component::Update(float deltaTime)
{
}
