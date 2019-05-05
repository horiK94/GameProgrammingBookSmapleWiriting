// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "AnimSpriteComponent.h"
#include "Math.h"

AnimSpriteComponent::AnimSpriteComponent(Actor* owner, int drawOrder)
	:SpriteComponent(owner, drawOrder)
	, mCurrFrame(0.0f)
	, mAnimFPS(24.0f)
{
}

void AnimSpriteComponent::Update(float deltaTime)
{
	SpriteComponent::Update(deltaTime);
	//設定されているアニメーションの最初, 最後のindexを取得
	std::vector<int> indexInfo = animIndex[currentAnimName];
	int firstIndex = indexInfo[0];
	int lastIndex = indexInfo[1];

	if (mAnimTextures.size() > 0)
	{
		// Update the current frame based on frame rate
		// and delta time
		mCurrFrame += mAnimFPS * deltaTime;
		
		// Wrap current frame if needed
		while (mCurrFrame >= lastIndex - firstIndex)
		{
			mCurrFrame -= (lastIndex - firstIndex);
		}

		// Set the current texture
		SetTexture(mAnimTextures[static_cast<int>(mCurrFrame + firstIndex)]);
	}
}

void AnimSpriteComponent::SetAnimTextures(std::string animName, const std::vector<SDL_Texture*>& textures)
{
	auto index = animIndex.find(animName);
	if (index != animIndex.end())
	{
		SDL_Log("すでに登録されているAnimation:%s を登録しようとしています", animName);
		return;
	}

	//登録するindexの番号を取得
	int registedMaxIndex = mAnimTextures.size();
	//textureの登録
	for (auto texture : textures)
	{
		mAnimTextures.push_back(texture);
	}
	//テクスチャ情報の登録
	animIndex[animName] = { registedMaxIndex, static_cast<int>(mAnimTextures.size()) };

	if (mAnimTextures.size() > 0)
	{
		// Set the active texture to first frame
		mCurrFrame = 0.0f;
		//アニメーションの設定
		currentAnimName = animName;
		//設定したアニメーションの最初にテクスチャを設定
		SetTexture(mAnimTextures[registedMaxIndex]);
	}
}

void AnimSpriteComponent::ChangeAnim(std::string animName)
{
	auto indexInfo = animIndex.find(animName);
	if (indexInfo == animIndex.end())
	{
		SDL_Log("引数の%sは、設定されていないアニメーション名です", animName);
		return;
	}

	mCurrFrame = 0.0f;
	currentAnimName = animName;

	int index = animIndex[animName][0];
	//設定したアニメーションの最初にテクスチャを設定
	SetTexture(mAnimTextures[index]);
}
