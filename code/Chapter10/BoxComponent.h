// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "Component.h"
#include "Collision.h"

class BoxComponent : public Component
{
public:
	BoxComponent(class Actor* owner, int updateOrder = 100);
	~BoxComponent();

	void OnUpdateWorldTransform() override;		//Update�ł͂Ȃ��B���L�A�N�^�[�����[���h�ϊ����Čv�Z����Ƃ��ɌĂ΂��

	void SetObjectBox(const AABB& model) { mObjectBox = model; }
	const AABB& GetWorldBox() const { return mWorldBox; }

	void SetShouldRotate(bool value) { mShouldRotate = value; }		//�A�N�^�[����]����Ƃ���BoxComponent����]�����邩�ݒ�
private:
	AABB mObjectBox;		//�I�u�W�F�N�g��Ԃ̃o�E���f�B���O�{�b�N�X
	AABB mWorldBox;			//���[���h��Ԃ̃o�E���f�B���O�{�b�N�X.���L�A�N�^�[�̃��[���h�ϊ��ɒǏ]
	bool mShouldRotate;
};
