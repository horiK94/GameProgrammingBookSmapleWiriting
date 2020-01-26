// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "SpriteComponent.h"
#include "Texture.h"
#include "Shader.h"
#include "Actor.h"
#include "Game.h"

SpriteComponent::SpriteComponent(Actor* owner, int drawOrder)
	:Component(owner)
	,mTexture(nullptr)
	,mDrawOrder(drawOrder)
	,mTexWidth(0)
	,mTexHeight(0)
{
	mOwner->GetGame()->AddSprite(this);
}

SpriteComponent::~SpriteComponent()
{
	mOwner->GetGame()->RemoveSprite(this);
}

void SpriteComponent::Draw(Shader* shader)
{
	//if (mTexture)
	//{
	//	// Scale the quad by the width/height of texture
	//	Matrix4 scaleMat = Matrix4::CreateScale(
	//		static_cast<float>(mTexWidth),
	//		static_cast<float>(mTexHeight),
	//		1.0f);
	//	
	//	Matrix4 world = scaleMat * mOwner->GetWorldTransform();
	//	
	//	// Since all sprites use the same shader/vertices,
	//	// the game first sets them active before any sprite draws
	//	
	//	// Set world transform
	//	shader->SetMatrixUniform("uWorldTransform", world);
	//	// Set current texture
	//	mTexture->SetActive();
	//	// Draw quad
	//	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	//}
	//テクスチャの幅と高さのスケーリング行列の作成
	Matrix4 scaleMat = Matrix4::CreateScale(
		static_cast<int>(mTexWidth),
		static_cast<int>(mTexHeight),
		1.0f);
	//スケーリング行列からワールド空間にするためワールド行列を掛ける
	//ワールド空間にする理由は140x140の画像に対して、actorがscale:2としていたら、280x280にするため
	Matrix4 world = scaleMat * mOwner->GetWorldTransform();

	//ワールド変換の設定
	shader->SetMatrixUniform("uWorldTransform", world);
	//テクスチャをアクティブに
	mTexture->SetActive();
	//スプライトの描画(描画前に頂点配列オブジェクトとシェーダーをアクティブにする必要あり)
	glDrawElements(
		GL_TRIANGLES,			//ポリゴンの種類
		6,						//インデックスバッファのいんでっくすの数
		GL_UNSIGNED_INT,		//インデックスの型
		nullptr);				//通常nullptr
}

void SpriteComponent::SetTexture(Texture* texture)
{
	mTexture = texture;
	// Set width/height
	mTexWidth = texture->GetWidth();
	mTexHeight = texture->GetHeight();
}
