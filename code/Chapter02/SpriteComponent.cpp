// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "SpriteComponent.h"
#include "Actor.h"
#include "Game.h"

SpriteComponent::SpriteComponent(Actor* owner, int drawOrder)
	:Component(owner)
	,mTexture(nullptr)
	,mDrawOrder(drawOrder)
	,mTexWidth(0)
	,mTexHeight(0)
{
	mOwner->GetGame()->AddSprite(this);		//自身を登録する
}

SpriteComponent::~SpriteComponent()
{
	mOwner->GetGame()->RemoveSprite(this);
}

void SpriteComponent::Draw(SDL_Renderer* renderer)
{
	if (mTexture)
	{
		SDL_Rect r;		//SDL_Rextのx, y座標は左上の座標のため、以下で補正を行う
		// Scale the width/height by owner's scale
		r.w = static_cast<int>(mTexWidth * mOwner->GetScale());		//横幅*サイズ
		r.h = static_cast<int>(mTexHeight * mOwner->GetScale());		//縦幅*サイズ
		// Center the rectangle around the position of the owner
		r.x = static_cast<int>(mOwner->GetPosition().x - r.w / 2);		//Actorが保持しているmPositionは中心点の座標のため、修正しなければならない
		r.y = static_cast<int>(mOwner->GetPosition().y - r.h / 2);

		// Draw (have to convert angle from radians to degrees, and clockwise to counter)
		SDL_RenderCopyEx(renderer,		//レンダラーターゲット
			mTexture,		//描画したいテクスチャ	
			nullptr,		//描画したいテクスチャの範囲(全体はnullptr)
			&r,				//描画範囲の矩形(要は、テクスチャはこの領域に合うよう拡大縮小を行う)
			-Math::ToDegrees(mOwner->GetRotation()),		//回転角(度, 時計回り)
			nullptr,		//回転中心(中央ならnullptr)
			SDL_FLIP_NONE);				//テクスチャを反転するか(反転しない場合は左のように設定する)
	}
}

//Textureを設定する
//Texture...画像そのもの
//Sprite..2Dグラフィカルオブジェクト. Texturenのここの部分を使用しますよといったことを指定するクラス
//Atlas化やコマ割りアニメとかのときは、Texture != Spriteの使用部分となる事が多い
void SpriteComponent::SetTexture(SDL_Texture* texture)
{
	mTexture = texture;
	// Set width/height
	SDL_QueryTexture(texture, nullptr, nullptr, &mTexWidth, &mTexHeight);		//Textureの情報の取得
	//簡単に言うと, 第１引数にTextureを代入すると、mTexWidth, mTexHeightが返される

	//Textureの描画は以下の2種類
	/*
	シンプル:
	int SDL_RenderCopy(SDL_Renderer* renderer, SDL_Texture* texture, const SDL_Rect* srcrect, const SDL_Rect* dstrect)
	SDL_Renderer* renderer:描画するレンダリングコンテキスト
	SDL_Texture* texture: 描画したいテクスチャ
	const SDL_Rect* srcrect: 描画したいテクスチャの範囲(すべてならnullptr)
	const SDL_Rect* dstrect: 描画範囲の矩形(要は、テクスチャはこの領域に合うよう拡大縮小を行う)
	
	スプライトを回転させたい場合は以下の関数を使用する

	*/
}
