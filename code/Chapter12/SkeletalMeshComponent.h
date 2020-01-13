// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "MeshComponent.h"
#include "MatrixPalette.h"

class SkeletalMeshComponent : public MeshComponent
{
public:
	SkeletalMeshComponent(class Actor* owner);
	// Draw this mesh component
	void Draw(class Shader* shader) override;

	void Update(float deltaTime) override;

	//// Setters
	//スケルトンのセット
	void SetSkeleton(const class Skeleton* sk) { mSkeleton = sk; }

	//// Play an animation. Returns the length of the animation
	//アニメーションの情報を保存し、フレーム0のときのポーズ行列を計算し、行列パレットに保存
	float PlayAnimation(const class Animation* anim, float playRate = 1.0f);
protected:
	//ボーンのグローバルな逆バインド行列と現在のグローバルポーズ行列を取得し、かけ合わせた結果をmPaletteに代入する
	void ComputeMatrixPalette();

	//逆バインドポーズ行列に現在のポーズ行列を掛けたもの(すきにんぐの頂点位置の計算に使用). ボーン数分の行列を持っている(現在の時刻の行列)
	MatrixPalette mPalette;
	//スケルトンの参照(バインドポーズ行列やボーン情報など)
	const class Skeleton* mSkeleton;
	//今再生しているアニメーションの情報
	const class Animation* mAnimation;

	//アニメーションの再生レート(スピード)
	float mAnimPlayRate;
	//アニメーションの現在時刻
	float mAnimTime;
};
