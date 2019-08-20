// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "AIComponent.h"
#include "Actor.h"
#include "AIState.h"
#include <SDL/SDL_log.h>

AIComponent::AIComponent(class Actor* owner)
:Component(owner)
,mCurrentState(nullptr)
{
}

void AIComponent::Update(float deltaTime)
{
	if (mCurrentState)
	{
		mCurrentState->Update(deltaTime);
	}
}

void AIComponent::ChangeState(const std::string& name)
{
	// First exit the current state
	if (mCurrentState)
	{
		mCurrentState->OnExit();		//���݂�state�����݂���ꍇ�Astate��OnExit()���Ă�
	}
	
	// Try to find the new state from the map
	auto iter = mStateMap.find(name);		//������name���L�[�Ƃ���State������
	if (iter != mStateMap.end())		//����������...
	{
		mCurrentState = iter->second;		//�l(IState�̃C���X�^���X)�̎擾
		// We're entering the new state
		mCurrentState->OnEnter();			//�V����state��OnEnter()���Ă�
	}
	else
	{		//������Ȃ�������
		SDL_Log("Could not find AIState %s in state map", name.c_str());
		mCurrentState = nullptr;
	}
}

void AIComponent::RegisterState(AIState* state)
{
	mStateMap.emplace(state->GetName(), state);
}
