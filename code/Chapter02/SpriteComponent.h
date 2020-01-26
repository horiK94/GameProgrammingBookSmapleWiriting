// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "Component.h"
#include "SDL/SDL.h"
class SpriteComponent : public Component
{
public:
	// (Lower draw order corresponds with further back)
	//コンストラクターの中でGame::AddSprite()を呼んで、スプライトコンポーネントの配列に登録する
	SpriteComponent(class Actor* owner, int drawOrder = 100);
	~SpriteComponent();

	virtual void Draw(SDL_Renderer* renderer);
	virtual void SetTexture(SDL_Texture* texture);

	//注; 描画順序が低いほど遠くに置かれる
	int GetDrawOrder() const { return mDrawOrder; }
	int GetTexHeight() const { return mTexHeight; }
	int GetTexWidth() const { return mTexWidth; }
protected:
	SDL_Texture* mTexture;		//Textureの参照
	int mDrawOrder;
	int mTexWidth;
	int mTexHeight;
};
