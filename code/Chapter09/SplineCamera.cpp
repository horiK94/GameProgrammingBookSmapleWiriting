// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "SplineCamera.h"
#include "Actor.h"

Vector3 Spline::Compute(size_t startIdx, float t) const
{
	//// Check if startIdx is out of bounds
	//if (startIdx >= mControlPoints.size())
	//{s
	//	return mControlPoints.back();
	//}
	//else if (startIdx == 0)
	//{
	//	return mControlPoints[startIdx];
	//}
	//else if (startIdx + 2 >= mControlPoints.size())
	//{
	//	return mControlPoints[startIdx];
	//}

	//// Get p0 through p3
	//Vector3 p0 = mControlPoints[startIdx - 1];
	//Vector3 p1 = mControlPoints[startIdx];
	//Vector3 p2 = mControlPoints[startIdx + 1];
	//Vector3 p3 = mControlPoints[startIdx + 2];
	//// Compute position according to Catmull-Rom equation
	//Vector3 position = 0.5f * ((2.0f * p1) + (-1.0f * p0 + p2) * t +
	//	(2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t * t +
	//	(-1.0f * p0 + 3.0f * p1 - 3.0f * p2 + p3) * t * t * t);
	//return position;

	//startIdxが境界外か
	if (startIdx >= mControlPoints.size())
	{
		//個数を超えた値を指定してきたら、最後の位置を返す
		return mControlPoints.back();
	}
	else if (startIdx == 0)
	{
		//最初の位置を指定してきたら一番最初の位置を返す
		return mControlPoints[startIdx];
	}
	else if (startIdx + 2 >= mControlPoints.size())
	{
		//個数に2個だけ小さい値を指定してきた場合、指定したindexの2個前の値を返す
	}

	// p0からp3までの制御点を取得する
	Vector3 p0 = mControlPoints[startIdx - 1];
	Vector3 p1 = mControlPoints[startIdx];
	Vector3 p2 = mControlPoints[startIdx + 1];
	Vector3 p3 = mControlPoints[startIdx + 2];

	// Catmull - Romの方程式によって、位置を計算する
	Vector3 position = 0.5 * (2 * p1 + (-1 * p0 + p2) * t + (2 * p0 - 5 * p1 + 4 * p2 - p3) * t * t
		+ (-1 * p0 + 3 * p1 - 3 * p2 + p3) * t * t * t);
	return position;
}

SplineCamera::SplineCamera(Actor* owner)
	:CameraComponent(owner)
	, mIndex(1)
	, mT(0.0f)
	, mSpeed(0.5f)
	, mPaused(true)
{
}

void SplineCamera::Update(float deltaTime)
{
	//CameraComponent::Update(deltaTime);
	//// Update t value
	//if (!mPaused)
	//{
	//	mT += mSpeed * deltaTime;
	//	// Advance to the next control point if needed.
	//	// This assumes speed isn't so fast that you jump past
	//	// multiple control points in one frame.
	//	if (mT >= 1.0f)
	//	{
	//		// Make sure we have enough points to advance the path
	//		if (mIndex < mPath.GetNumPoints() - 3)
	//		{
	//			mIndex++;
	//			mT = mT - 1.0f;
	//		}
	//		else
	//		{
	//			// Path's done, so pause
	//			mPaused = true;
	//		}
	//	}
	//}

	//// Camera position is the spline at the current t/index
	//Vector3 cameraPos = mPath.Compute(mIndex, mT);
	//// Target point is just a small delta ahead on the spline
	//Vector3 target = mPath.Compute(mIndex, mT + 0.01f);
	//// Assume spline doesn't flip upside-down
	//const Vector3 up = Vector3::UnitZ;
	//Matrix4 view = Matrix4::CreateLookAt(cameraPos, target, up);
	//SetViewMatrix(view);

	CameraComponent::Update(deltaTime);
	//tの値を更新
	if (!mPaused)
	{
		mT += deltaTime * mSpeed;
		//必要ならば次の制御点に進む。
		//スピードが早すぎて1フレームで複数の制御点を超えることがないことが前提
		if (mT >= 1.0f)
		{
			if (mIndex < mPath.GetNumPoints() - 3)
			{
				//次の制御点が存在するなら、初期化する
				mIndex++;
				mT -= 1.0f;
			}
			else
			{
				mPaused = true;
			}
		}
	}

	//カメラの位置を、現在のインデックスとtから求める
	Vector3 cameraPos = mPath.Compute(mIndex, mT);
	//注視点はtをわずかに小さくした先
	Vector3 targetPos = mPath.Compute(mIndex, mT + 0.01f);
	//スプラインを上下逆にしないことを前提にする
	Vector3 up = Vector3::UnitZ;
	Matrix4 view = Matrix4::CreateLookAt(cameraPos, targetPos, up);
	SetViewMatrix(view);
}

void SplineCamera::Restart()
{
	mIndex = 1;
	mT = 0.0f;
	mPaused = false;
}
