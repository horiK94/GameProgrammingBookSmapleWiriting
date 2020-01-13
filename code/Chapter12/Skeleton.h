// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "BoneTransform.h"
#include <string>
#include <vector>

class Skeleton
{
public:
	// Definition for each bone in the skeleton
	struct Bone
	{
		//�ʒu���]�Ȃ�(�e����̑��΋���)
		BoneTransform mLocalBindPose;
		//���O
		std::string mName;
		//�e�{�[���̃C���f�b�N�X
		int mParent;
	};

	// Load from a file
	bool Load(const std::string& fileName);

	// Getter functions
	size_t GetNumBones() const { return mBones.size(); }
	const Bone& GetBone(size_t idx) const { return mBones[idx]; }
	const std::vector<Bone>& GetBones() const { return mBones; }
	const std::vector<Matrix4>& GetGlobalInvBindPoses() const { return mGlobalInvBindPoses; }
protected:
	// Called automatically when the skeleton is loaded
	// Computes the global inverse bind pose for each bone
	//�S�Ẵ{�[���̃O���[�o���ȋt�o�C���h�|�[�Y�s����v�Z����mGlobalInvBindPoses�ɕۑ�
	void ComputeGlobalInvBindPose();
private:
	// The bones in the skeleton
	//�X�P���g���̃{�[��. �e�͎q���index���������Ƃ������[��������Ă���
	std::vector<Bone> mBones;
	// The global inverse bind poses for each bone
	// �{�[���̃O���[�o���ȋt�o�C���h�|�[�Y�s��(�e�{�[���ɑ΂��ĕۑ�)
	std::vector<Matrix4> mGlobalInvBindPoses;
};
