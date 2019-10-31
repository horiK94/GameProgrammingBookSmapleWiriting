// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "FollowCamera.h"
#include "Actor.h"

FollowCamera::FollowCamera(Actor* owner)
	:CameraComponent(owner)
	, mHorzDist(350.0f)
	, mVertDist(150.0f)
	, mTargetDist(100.0f)
	, mSpringConstant(64.0f)
{
}

void FollowCamera::Update(float deltaTime)
{
	//CameraComponent::Update(deltaTime);
	//// Compute dampening from spring constant
	//float dampening = 2.0f * Math::Sqrt(mSpringConstant);
	//// Compute ideal position
	//Vector3 idealPos = ComputeCameraPos();
	//// Compute difference between actual and ideal
	//Vector3 diff = mActualPos - idealPos;
	//// Compute acceleration of spring
	//Vector3 acel = -mSpringConstant * diff -
	//	dampening * mVelocity;
	//// Update velocity
	//mVelocity += acel * deltaTime;
	//// Update actual camera position
	//mActualPos += mVelocity * deltaTime;
	//// Target is target dist in front of owning actor
	//Vector3 target = mOwner->GetPosition() +
	//	mOwner->GetForward() * mTargetDist;
	//// Use actual position here, not ideal
	//Matrix4 view = Matrix4::CreateLookAt(mActualPos, target,
	//	Vector3::UnitZ);
	//SetViewMatrix(view);

	//�J�����ʒu�ƒ����_�����߂āA�r���[�s��𓾂�
	//CameraComponent::Update(deltaTime);
	//Vector3 target = mOwner->GetPosition() + mOwner->GetForward() * mTargetDist;
	//Matrix4 view = Matrix4::CreateLookAt(ComputeCameraPos(), target, Vector3::UnitZ);
	//SetViewMatrix(view);

	CameraComponent::Update(deltaTime);

	//�΂˒萔���猸�����v�Z
	float dampening = 2.0f * Math::Sqrt(mSpringConstant);

	//���z�ʒu�̎擾
	Vector3 idealPos = ComputeCameraPos();

	//���ۂƗ��z�̍����v�Z(���ۂ̈ʒu�͑O�t���[���̒l���g�p)
	Vector3 diff = mActualPos - idealPos;
	//�΂˂ɂ������x���v�Z
	Vector3 acel = -mSpringConstant * diff - dampening * mVelocity;

	//���x�̍X�V
	mVelocity += acel * deltaTime;
	//���ۂ̃J�����ʒu�̍X�V
	mActualPos += mVelocity * deltaTime;

	//�^�[�Q�b�g�͏��L�A�N�^�[�̑O���ɂ���ڕW�_
	Vector3 target = mOwner->GetPosition() + mOwner->GetForward() * mTargetDist;
	Matrix4 view = Matrix4::CreateLookAt(/*ComputeCameraPos()*/mActualPos, target, Vector3::UnitZ);
	SetViewMatrix(view);
}

//FollowActor�����������ɌĂԊ֐�
void FollowCamera::SnapToIdeal()
{
	// Set actual position to ideal
	//���ۂ̈ʒu�𗝑z�̈ʒu�͓���
	mActualPos = ComputeCameraPos();
	// Zero velocity
	//���x0
	mVelocity = Vector3::Zero;
	// Compute target and view
	//�����_�ƃr���[�v�Z
	Vector3 target = mOwner->GetPosition() +
		mOwner->GetForward() * mTargetDist;
	// Use actual position here, not ideal
	Matrix4 view = Matrix4::CreateLookAt(mActualPos, target,
		Vector3::UnitZ);
	SetViewMatrix(view);
}

Vector3 FollowCamera::ComputeCameraPos() const
{
	// Set camera position behind and above owner
	//Vector3 cameraPos = mOwner->GetPosition();
	//cameraPos -= mOwner->GetForward() * mHorzDist;
	//cameraPos += Vector3::UnitZ * mVertDist;
	//return cameraPos;
	Vector3 cameraPos = mOwner->GetPosition();
	cameraPos -= mOwner->GetForward() * mHorzDist;
	cameraPos += Vector3::UnitZ * mVertDist;
	return cameraPos;
}
