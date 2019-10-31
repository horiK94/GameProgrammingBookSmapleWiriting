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

//�Ȑ����
struct Spline
{
	// Control points for spline
	// (Requires n+2 points where n is number
	// of points in segment)
	//�X�v���C���̂��߂̐���_(N�̋Ȑ��ɑ�����_������Ȃ�AN+2�̏�񂪕K�v)
	std::vector<Vector3> mControlPoints;
	// Given spline segment where startIdx = P1,
	// compute position based on t value
	// p1 = mControllerPoints[startIdx]�Ƃ��čl�����Ƃ��̋�ԂŁAt�̒l�Ɋ�Â��Ĉʒu���v�Z
	Vector3 Compute(size_t startIdx, float t) const;
	// Returns number of control points
	//�_�̐���Ԃ�
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
	//�J�������Ǐ]����X�v���C���o�H
	Spline mPath;
	// Current control point index and t
	//���݂̐���_�̃C���f�b�N�X
	size_t mIndex;
	//���݂�t�̒l
	float mT;
	// Amount t changes/sec
	//�X�s�[�h = t�̑���/�b
	float mSpeed;
	// Whether to move the camera long the path
	//�J�������o�H�ɉ����ē�������
	bool mPaused;
};
