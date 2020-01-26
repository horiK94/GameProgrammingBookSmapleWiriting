// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "BoneTransform.h"
#include <string>
#include <vector>

class Skeleton
{
public:
	// Definition for each bone in the skeleton
	struct Bone
	{
		//位置や回転など(親からの相対距離)
		BoneTransform mLocalBindPose;
		//名前
		std::string mName;
		//親ボーンのインデックス
		int mParent;
	};

	// Load from a file
	bool Load(const std::string& fileName);

	// Getter functions
	size_t GetNumBones() const { return mBones.size(); }
	const Bone& GetBone(size_t idx) const { return mBones[idx]; }
	const std::vector<Bone>& GetBones() const { return mBones; }
	const std::vector<Matrix4>& GetGlobalInvBindPoses() const { return mGlobalInvBindPoses; }
protected:
	// Called automatically when the skeleton is loaded
	// Computes the global inverse bind pose for each bone
	//全てのボーンのグローバルな逆バインドポーズ行列を計算してmGlobalInvBindPosesに保存
	void ComputeGlobalInvBindPose();
private:
	// The bones in the skeleton
	//スケルトンのボーン. 親は子よりindexが小さいというルールが守られている
	std::vector<Bone> mBones;
	// The global inverse bind poses for each bone
	// ボーンのグローバルな逆バインドポーズ行列(各ボーンに対して保存)
	std::vector<Matrix4> mGlobalInvBindPoses;
};
