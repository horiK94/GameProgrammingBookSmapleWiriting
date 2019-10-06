// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "AudioComponent.h"
#include "Actor.h"
#include "Game.h"
#include "AudioSystem.h"

AudioComponent::AudioComponent(Actor* owner, int updateOrder)
	:Component(owner, updateOrder)
{
}

AudioComponent::~AudioComponent()
{
	StopAllEvents();
}

void AudioComponent::Update(float deltaTime)
{
	Component::Update(deltaTime);

	// Remove invalid 2D events
	auto iter = mEvents2D.begin();
	while (iter != mEvents2D.end())
	{
		if (!iter->IsValid())
		{
			//eventが無効になっていたら
			iter = mEvents2D.erase(iter);		//削除された要素の次の要素を指すイテレータを返す
		}
		else
		{
			++iter;		//削次の要素を指すイテレータを返す
		}
	}

	// Remove invalid 3D events
	iter = mEvents3D.begin();
	while (iter != mEvents3D.end())
	{
		if (!iter->IsValid())
		{
			iter = mEvents3D.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

//所有アクターがワールド行列を計算するたびにこの関数が呼ばれる
void AudioComponent::OnUpdateWorldTransform()
{
	//// Update 3D events' world transforms
	//Matrix4 world = mOwner->GetWorldTransform();
	//for (auto& event : mEvents3D)
	//{
	//	if (event.IsValid())
	//	{
	//		event.Set3DAttributes(world);
	//	}
	//}
	Matrix4 world = mOwner->GetWorldTransform();
	for (auto& event : mEvents3D)
	{
		if (event.IsValid())
		{
			event.Set3DAttributes(world);
		}
	}
}

SoundEvent AudioComponent::PlayEvent(const std::string& name)
{
	//SoundEvent e = mOwner->GetGame()->GetAudioSystem()->PlayEvent(name);
	//// Is this 2D or 3D?
	//if (e.Is3D())
	//{
	//	mEvents3D.emplace_back(e);
	//	// Set initial 3D attributes
	//	e.Set3DAttributes(mOwner->GetWorldTransform());
	//}
	//else
	//{
	//	mEvents2D.emplace_back(e);
	//}
	//return e;
	SoundEvent e = mOwner->GetGame()->GetAudioSystem()->PlayEvent(name);
	if (e.Is3D())
	{
		//3Dの時
		mEvents3D.emplace_back(e);
		//3D属性を初期化する
		e.Set3DAttributes(mOwner->GetWorldTransform());
	}
	else
	{
		//2Dの時
		mEvents2D.emplace_back(e);
	}
	return e;
}

void AudioComponent::StopAllEvents()
{
	// Stop all sounds
	for (auto& e : mEvents2D)
	{
		e.Stop();
	}
	for (auto& e : mEvents3D)
	{
		e.Stop();
	}
	// Clear events
	mEvents2D.clear();
	mEvents3D.clear();
}
