// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include <string>
#include <unordered_map>
#include <SDL/SDL_ttf.h>
#include "Math.h"

//�����̃t�H���g���Ǘ�����N���X�ł͂Ȃ��A�P��̃t�H���g���Ǘ�
class Font
{
public:
	Font(class Game* game);
	~Font();
	
	// Load/unload from a file
	//�t�@�C������t�H���g�����[�h
	bool Load(const std::string& fileName);
	//�f�[�^�J��
	void Unload();
	
	// Given string and this font, draw to a texture
	//������A�F�A�|�C���^�T�C�Y��n���ƃe�L�X�g���܂ރe�N�X�`�����쐬�����
	class Texture* RenderText(const std::string& textKey,
							  const Vector3& color = Color::White,
							  int pointSize = 30);
private:
	// Map of point sizes to font data
	std::unordered_map<int, TTF_Font*> mFontData;
	class Game* mGame;
};
