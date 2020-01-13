// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "SkeletalMeshComponent.h"
#include "Shader.h"
#include "Mesh.h"
#include "Actor.h"
#include "Game.h"
#include "Renderer.h"
#include "Texture.h"
#include "VertexArray.h"
#include "Animation.h"
#include "Skeleton.h"

SkeletalMeshComponent::SkeletalMeshComponent(Actor* owner)
	:MeshComponent(owner, true)
	,mSkeleton(nullptr)
{
}

void SkeletalMeshComponent::Draw(Shader* shader)
{
	if (mMesh)
	{
		// Set the world transform
		shader->SetMatrixUniform("uWorldTransform", 
			mOwner->GetWorldTransform());
		// Set the matrix palette
		//SetMatrixUniforms��uniform�̖��O�ƁA�s��̃|�C���^��GPU�ɑ���s��̐��������ɓn��
		shader->SetMatrixUniforms("uMatrixPalette", &mPalette.mEntry[0],		//mPallete�̃|�C���^�𑗂�
			MAX_SKELETON_BONES);
		// Set specular power
		shader->SetFloatUniform("uSpecPower", mMesh->GetSpecPower());
		// Set the active texture
		Texture* t = mMesh->GetTexture(mTextureIndex);
		if (t)
		{
			t->SetActive();
		}
		// Set the mesh's vertex array as active
		VertexArray* va = mMesh->GetVertexArray();
		va->SetActive();
		// Draw
		glDrawElements(GL_TRIANGLES, va->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
	}
}

void SkeletalMeshComponent::Update(float deltaTime)
{
	//mAnimation���Đ����[�g�ɏ]���čX�V
	if (mAnimation && mSkeleton)		//�X�P���g���f�[�^�������̓A�j���[�V�����f�[�^���Ȃ��ꍇ�͍X�V���Ȃ�
	{
		mAnimTime += deltaTime * mAnimPlayRate;
		// Wrap around anim time if past duration
		while (mAnimTime > mAnimation->GetDuration())
		{
			//�A�j���[�V�����̒����𒴂����ꍇ�́A����������mAnimTime�����炷
			mAnimTime -= mAnimation->GetDuration();
		}

		// Recompute matrix palette
		//�������̍s��p���b�g���擾
		ComputeMatrixPalette();
	}
}

//�A�j���[�V�����̒�����Ԃ�
float SkeletalMeshComponent::PlayAnimation(const Animation* anim, float playRate)
{
	//������SkeletalMeshComponent�̕ϐ��ɑ��
	mAnimation = anim;
	mAnimTime = 0.0f;		//�Đ����Ԃ�0�ɂ���
	mAnimPlayRate = playRate;

	if (!mAnimation) { return 0.0f; }		//mAnimation��null�Ȃ�A�j���[�V�����̒�����0�ŕԂ�

	//0s�ڂ̂Ƃ��̋t�o�C���h�|�[�Y�s��Ɍ��݂̃|�[�Y�s��(�O���[�o���|�[�Y�s��)���������l���v�Z���ĕۑ�
	ComputeMatrixPalette();

	//mAnimation�̒���Animation�̒�����Ԃ�
	return mAnimation->GetDuration();
}

void SkeletalMeshComponent::ComputeMatrixPalette()
{
	//�O���[�o���t�o�C���h�|�[�Y�s��̎擾
	const std::vector<Matrix4>& globalInvBindPoses = mSkeleton->GetGlobalInvBindPoses();
	//���݂̃|�[�Y�s��̏���
	std::vector<Matrix4> currentPoses;
	//mAnimTime�̂Ƃ��̃|�[�Y�s���currentPoses�ŕԂ��Ă��炤
	mAnimation->GetGlobalPoseAtTime(currentPoses, mSkeleton, mAnimTime);

	// Setup the palette for each bone
	//�e�{�[���̃p���b�g�ݒ�(�t�o�C���h�|�[�Y�s��Ɍ��݂̃O���[�o���|�[�Y�s�������������)
	for (size_t i = 0; i < mSkeleton->GetNumBones(); i++)
	{
		// Global inverse bind pose matrix times current pose matrix
		mPalette.mEntry[i] = globalInvBindPoses[i] * currentPoses[i];
	}
}
