// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "SkeletalMeshComponent.h"
#include "Shader.h"
#include "Mesh.h"
#include "Actor.h"
#include "Game.h"
#include "Renderer.h"
#include "Texture.h"
#include "VertexArray.h"
#include "Animation.h"
#include "Skeleton.h"

SkeletalMeshComponent::SkeletalMeshComponent(Actor* owner)
	:MeshComponent(owner, true)
	,mSkeleton(nullptr)
{
}

void SkeletalMeshComponent::Draw(Shader* shader)
{
	if (mMesh)
	{
		// Set the world transform
		shader->SetMatrixUniform("uWorldTransform", 
			mOwner->GetWorldTransform());
		// Set the matrix palette
		//SetMatrixUniformsはuniformの名前と、行列のポインタとGPUに送る行列の数を引数に渡す
		shader->SetMatrixUniforms("uMatrixPalette", &mPalette.mEntry[0],		//mPalleteのポインタを送る
			MAX_SKELETON_BONES);
		// Set specular power
		shader->SetFloatUniform("uSpecPower", mMesh->GetSpecPower());
		// Set the active texture
		Texture* t = mMesh->GetTexture(mTextureIndex);
		if (t)
		{
			t->SetActive();
		}
		// Set the mesh's vertex array as active
		VertexArray* va = mMesh->GetVertexArray();
		va->SetActive();
		// Draw
		glDrawElements(GL_TRIANGLES, va->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
	}
}

void SkeletalMeshComponent::Update(float deltaTime)
{
	//mAnimationを再生レートに従って更新
	if (mAnimation && mSkeleton)		//スケルトンデータもしくはアニメーションデータがない場合は更新しない
	{
		mAnimTime += deltaTime * mAnimPlayRate;
		// Wrap around anim time if past duration
		while (mAnimTime > mAnimation->GetDuration())
		{
			//アニメーションの長さを超えた場合は、長さ分だけmAnimTimeを減らす
			mAnimTime -= mAnimation->GetDuration();
		}

		// Recompute matrix palette
		//現時刻の行列パレットを取得
		ComputeMatrixPalette();
	}
}

//アニメーションの長さを返す
float SkeletalMeshComponent::PlayAnimation(const Animation* anim, float playRate)
{
	//引数をSkeletalMeshComponentの変数に代入
	mAnimation = anim;
	mAnimTime = 0.0f;		//再生時間を0にする
	mAnimPlayRate = playRate;

	if (!mAnimation) { return 0.0f; }		//mAnimationがnullならアニメーションの長さは0で返す

	//0s目のときの逆バインドポーズ行列に現在のポーズ行列(グローバルポーズ行列)をかけた値を計算して保存
	ComputeMatrixPalette();

	//mAnimationの中のAnimationの長さを返す
	return mAnimation->GetDuration();
}

void SkeletalMeshComponent::ComputeMatrixPalette()
{
	//グローバル逆バインドポーズ行列の取得
	const std::vector<Matrix4>& globalInvBindPoses = mSkeleton->GetGlobalInvBindPoses();
	//現在のポーズ行列の所得
	std::vector<Matrix4> currentPoses;
	//mAnimTimeのときのポーズ行列をcurrentPosesで返してもらう
	mAnimation->GetGlobalPoseAtTime(currentPoses, mSkeleton, mAnimTime);

	// Setup the palette for each bone
	//各ボーンのパレット設定(逆バインドポーズ行列に現在のグローバルポーズ行列をかけたもの)
	for (size_t i = 0; i < mSkeleton->GetNumBones(); i++)
	{
		// Global inverse bind pose matrix times current pose matrix
		mPalette.mEntry[i] = globalInvBindPoses[i] * currentPoses[i];
	}
}
