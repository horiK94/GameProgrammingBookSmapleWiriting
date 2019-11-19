// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "BoxComponent.h"
#include "Actor.h"
#include "Game.h"
#include "PhysWorld.h"

BoxComponent::BoxComponent(Actor* owner, int updateOrder)
	:Component(owner, updateOrder)
	,mObjectBox(Vector3::Zero, Vector3::Zero)
	,mWorldBox(Vector3::Zero, Vector3::Zero)
	,mShouldRotate(true)
{
	//mOwner->GetGame()->GetPhysWorld()->AddBox(this);
	mOwner->GetGame()->GetPhysWorld()->AddBox(this);		//���̃R���|�[�l���g���ABoxComponent���Ǘ����Ă���PhysWorld��Add
}

BoxComponent::~BoxComponent()
{
	//mOwner->GetGame()->GetPhysWorld()->RemoveBox(this);
	mOwner->GetGame()->GetPhysWorld()->RemoveBox(this);
}

void BoxComponent::OnUpdateWorldTransform()
{
	//// Reset to object space box
	//mWorldBox = mObjectBox;
	//// Scale
	//mWorldBox.mMin *= mOwner->GetScale();
	//mWorldBox.mMax *= mOwner->GetScale();
	//// Rotate (if we want to)
	//if (mShouldRotate)
	//{
	//	mWorldBox.Rotate(mOwner->GetRotation());
	//}
	//// Translate
	//mWorldBox.mMin += mOwner->GetPosition();
	//mWorldBox.mMax += mOwner->GetPosition();

	//�I�u�W�F�N�g��Ԃ��X�P�[�����O�A��]�A���s�ړ���K�p���A���[���h���W�ɂ���
	mWorldBox = mObjectBox;
	//�X�P�[�����O�K�p
	mWorldBox.mMin *= mOwner->GetScale();
	mWorldBox.mMax *= mOwner->GetScale();

	//��]�K�p
	if (mShouldRotate)
	{
		//��]������ݒ�Ȃ珊�L�A�N�^�[��Quaternion��n��
		mWorldBox.Rotate(mOwner->GetRotation());
	}
	//���s�ړ�
	mWorldBox.mMin += mOwner->GetPosition();
	mWorldBox.mMax += mOwner->GetPosition();
}
