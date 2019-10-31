// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "OrbitCamera.h"
#include "Actor.h"

OrbitCamera::OrbitCamera(Actor* owner)
	:CameraComponent(owner)
	//,mOffset(-400.0f, 0.0f, 0.0f)
	//,mUp(Vector3::UnitZ)
	,mOffset(-400, 0, 0)
	,mUp(Vector3::UnitZ)
	,mPitchSpeed(0.0f)
	,mYawSpeed(0.0f)
{
}

void OrbitCamera::Update(float deltaTime)
{
	//CameraComponent::Update(deltaTime);
	//// Create a quaternion for yaw about world up
	//Quaternion yaw(Vector3::UnitZ, mYawSpeed * deltaTime);
	//// Transform offset and up by yaw
	//mOffset = Vector3::Transform(mOffset, yaw);
	//mUp = Vector3::Transform(mUp, yaw);

	//// Compute camera forward/right from these vectors
	//// Forward owner.position - (owner.position + offset)
	//// = -offset
	//Vector3 forward = -1.0f * mOffset;
	//forward.Normalize();
	//Vector3 right = Vector3::Cross(mUp, forward);
	//right.Normalize();
	//
	//// Create quaternion for pitch about camera right
	//Quaternion pitch(right, mPitchSpeed * deltaTime);
	//// Transform camera offset and up by pitch
	//mOffset = Vector3::Transform(mOffset, pitch);
	//mUp = Vector3::Transform(mUp, pitch);

	//// Compute transform matrix
	//Vector3 target = mOwner->GetPosition();
	//Vector3 cameraPos = target + mOffset;
	//Matrix4 view = Matrix4::CreateLookAt(cameraPos, target, mUp);
	//SetViewMatrix(view);
	
	CameraComponent::Update(deltaTime);
	//���[���h��������Ƃ��郈�[�̃N�I�[�^�j�I�����쐬
	Quaternion yaw(Vector3::UnitZ, mYawSpeed * deltaTime);		//UnitZ�����ɁAyYawSpeed * deltaTime��]
	//offset�Ə���x�N�g�������[��p���Ď擾
	mOffset = Vector3::Transform(mOffset, yaw);
	mUp = Vector3::Transform(mUp, yaw);

	//�O���x�N�g���� owner.position - (owner.position + offset) = -offset ���
	Vector3 forward = -1 * mOffset;
	forward.Normalize();
	//�E�x�N�g����
	Vector3 right = Vector3::Cross(forward, mUp);
	right.Normalize();

	Quaternion pitch(right, mPitchSpeed * deltaTime);
	//�I�t�Z�b�g�Ə�����s�b�`�ŕϊ�
	mOffset = Vector3::Transform(mOffset, pitch);
	mUp = Vector3::Transform(mUp, pitch);
	
	//�ϊ��s����v�Z
	Vector3 target = mOwner->GetForward();
	Vector3 cameraPos = target + mOffset;
	Matrix4 view = Matrix4::CreateLookAt(cameraPos, target, mUp);
	SetViewMatrix(view);
}
