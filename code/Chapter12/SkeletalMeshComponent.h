// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "MeshComponent.h"
#include "MatrixPalette.h"

class SkeletalMeshComponent : public MeshComponent
{
public:
	SkeletalMeshComponent(class Actor* owner);
	// Draw this mesh component
	void Draw(class Shader* shader) override;

	void Update(float deltaTime) override;

	//// Setters
	//�X�P���g���̃Z�b�g
	void SetSkeleton(const class Skeleton* sk) { mSkeleton = sk; }

	//// Play an animation. Returns the length of the animation
	//�A�j���[�V�����̏���ۑ����A�t���[��0�̂Ƃ��̃|�[�Y�s����v�Z���A�s��p���b�g�ɕۑ�
	float PlayAnimation(const class Animation* anim, float playRate = 1.0f);
protected:
	//�{�[���̃O���[�o���ȋt�o�C���h�s��ƌ��݂̃O���[�o���|�[�Y�s����擾���A�������킹�����ʂ�mPalette�ɑ������
	void ComputeMatrixPalette();

	//�t�o�C���h�|�[�Y�s��Ɍ��݂̃|�[�Y�s����|��������(�����ɂ񂮂̒��_�ʒu�̌v�Z�Ɏg�p). �{�[�������̍s��������Ă���(���݂̎����̍s��)
	MatrixPalette mPalette;
	//�X�P���g���̎Q��(�o�C���h�|�[�Y�s���{�[�����Ȃ�)
	const class Skeleton* mSkeleton;
	//���Đ����Ă���A�j���[�V�����̏��
	const class Animation* mAnimation;

	//�A�j���[�V�����̍Đ����[�g(�X�s�[�h)
	float mAnimPlayRate;
	//�A�j���[�V�����̌��ݎ���
	float mAnimTime;
};
