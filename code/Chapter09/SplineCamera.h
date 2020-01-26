// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "CameraComponent.h"
#include <vector>

//曲線情報
struct Spline
{
	// Control points for spline
	// (Requires n+2 points where n is number
	// of points in segment)
	//スプラインのための制御点(N個の曲線に属する点があるなら、N+2個の情報が必要)
	std::vector<Vector3> mControlPoints;
	// Given spline segment where startIdx = P1,
	// compute position based on t value
	// p1 = mControllerPoints[startIdx]として考えたときの区間で、tの値に基づいて位置を計算
	Vector3 Compute(size_t startIdx, float t) const;
	// Returns number of control points
	//点の数を返す
	size_t GetNumPoints() const { return mControlPoints.size(); }
};

class SplineCamera : public CameraComponent
{
public:
	SplineCamera(class Actor* owner);

	void Update(float deltaTime) override;
	// Restart the spline
	void Restart();

	void SetSpeed(float speed) { mSpeed = speed; }
	void SetSpline(const Spline& spline) { mPath = spline; }
	void SetPaused(bool pause) { mPaused = pause; }
private:
	// Spline path camera follows
	//カメラが追従するスプライン経路
	Spline mPath;
	// Current control point index and t
	//現在の制御点のインデックス
	size_t mIndex;
	//現在のtの値
	float mT;
	// Amount t changes/sec
	//スピード = tの増分/秒
	float mSpeed;
	// Whether to move the camera long the path
	//カメラを経路に沿って動かすか
	bool mPaused;
};
