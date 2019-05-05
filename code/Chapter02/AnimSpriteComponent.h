// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "SpriteComponent.h"
#include <vector>
#include <map>
#include <string>
class AnimSpriteComponent : public SpriteComponent
{
public:
	AnimSpriteComponent(class Actor* owner, int drawOrder = 100);
	// Update animation every frame (overriden from component)
	void Update(float deltaTime) override;
	// Set the textures used for animation
	void SetAnimTextures(std::string animName, const std::vector<SDL_Texture*>& textures);
	// Set/get the animation FPS
	float GetAnimFPS() const { return mAnimFPS; }
	void SetAnimFPS(float fps) { mAnimFPS = fps; }
	//�A�j���[�V�����̕ύX
	void ChangeAnim(std::string animName);
private:
	// All textures in the animation
	std::vector<SDL_Texture*> mAnimTextures;
	//�A�j���[�V�����ɑΉ�����C���f�b�N�X
	std::map<std::string, std::vector<int>> animIndex;
	//���ݍĐ����̃A�j���[�V����
	std::string currentAnimName;
	// Current frame displayed
	float mCurrFrame;
	// Animation frame rate
	float mAnimFPS;
};
