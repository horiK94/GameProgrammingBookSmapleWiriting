// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "Math.h"

class BoneTransform
{
public:
	// For now, just make this data public
	//回転行列
	Quaternion mRotation;
	//位置
	Vector3 mTranslation;
	//行列に変換
	Matrix4 ToMatrix() const;

	static BoneTransform Interpolate(const BoneTransform& a, const BoneTransform& b, float f);
};
