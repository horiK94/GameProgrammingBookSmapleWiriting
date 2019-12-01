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

//複数のフォントを管理するクラスではなく、単一のフォントを管理
class Font
{
public:
	Font(class Game* game);
	~Font();
	
	// Load/unload from a file
	//ファイルからフォントをロード
	bool Load(const std::string& fileName);
	//データ開放
	void Unload();
	
	// Given string and this font, draw to a texture
	//文字列、色、ポインタサイズを渡すとテキストを含むテクスチャが作成される
	class Texture* RenderText(const std::string& textKey,
							  const Vector3& color = Color::White,
							  int pointSize = 30);
private:
	// Map of point sizes to font data
	std::unordered_map<int, TTF_Font*> mFontData;
	class Game* mGame;
};
