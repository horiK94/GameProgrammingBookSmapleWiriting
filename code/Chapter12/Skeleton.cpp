// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Skeleton.h"
#include <fstream>
#include <sstream>
#include <rapidjson/document.h>
#include <SDL/SDL_log.h>
#include "MatrixPalette.h"

bool Skeleton::Load(const std::string& fileName)
{
	std::ifstream file(fileName);
	if (!file.is_open())
	{
		SDL_Log("File not found: Skeleton %s", fileName.c_str());
		return false;
	}

	//file����stringstream�ɕϊ����A���g��string�Ɉڂ�����Ajson��string�ɕϊ�
	std::stringstream fileStream;
	fileStream << file.rdbuf();
	std::string contents = fileStream.str();
	rapidjson::StringStream jsonStr(contents.c_str());
	rapidjson::Document doc;
	//jsonStr��DOM Tree�̌`�ɕϊ����Ă���
	doc.ParseStream(jsonStr);

	if (!doc.IsObject())
	{
		//DOM Tree�̌`�ɂł��Ȃ�����
		SDL_Log("Skeleton %s is not valid json", fileName.c_str());
		return false;
	}

	int ver = doc["version"].GetInt();

	// Check the metadata
	if (ver != 1)
	{
		//version��1����Ȃ�����
		SDL_Log("Skeleton %s unknown format", fileName.c_str());
		return false;
	}

	const rapidjson::Value& bonecount = doc["bonecount"];
	if (!bonecount.IsUint())
	{
		//bone�̐�(bonecount)��uint�ł͂Ȃ�����
		SDL_Log("Skeleton %s doesn't have a bone count.", fileName.c_str());
		return false;
	}

	size_t count = bonecount.GetUint();

	if (count > MAX_SKELETON_BONES)
	{
		//�{�[���������̒l�𒴂��Ă���ꍇ
		SDL_Log("Skeleton %s exceeds maximum bone count.", fileName.c_str());
		return false;
	}

	// mBones�z���count�̃T�C�Y�ɍč\�z
	mBones.reserve(count);

	const rapidjson::Value& bones = doc["bones"];
	if (!bones.IsArray())
	{
		//bones���z��łȂ�
		SDL_Log("Skeleton %s doesn't have a bone array?", fileName.c_str());
		return false;
	}

	if (bones.Size() != count)
	{
		//bones�z��̃T�C�Y��count�ƈ�v���Ȃ�
		SDL_Log("Skeleton %s has a mismatch between the bone count and number of bones", fileName.c_str());
		return false;
	}

	Bone temp;

	//�{�[���̐������T������
	for (rapidjson::SizeType i = 0; i < count; i++)
	{
		if (!bones[i].IsObject())
		{
			//bones�̒���DOM tree�ɂł��Ȃ������Ƃ�
			SDL_Log("Skeleton %s: Bone %d is invalid.", fileName.c_str(), i);
			return false;
		}

		const rapidjson::Value& name = bones[i]["name"];
		temp.mName = name.GetString();

		const rapidjson::Value& parent = bones[i]["parent"];
		temp.mParent = parent.GetInt();

		const rapidjson::Value& bindpose = bones[i]["bindpose"];
		if (!bindpose.IsObject())
		{
			//bindpose�̒���DOM Tree�ɂł��Ȃ������Ƃ�
			SDL_Log("Skeleton %s: Bone %d is invalid.", fileName.c_str(), i);
			return false;
		}

		const rapidjson::Value& rot = bindpose["rot"];
		const rapidjson::Value& trans = bindpose["trans"];

		if (!rot.IsArray() || !trans.IsArray())
		{
			//rot��trans���z��ł���Ƃ�����Ԃł͂Ȃ������Ƃ�
			SDL_Log("Skeleton %s: Bone %d is invalid.", fileName.c_str(), i);
			return false;
		}

		//��]��ʒu�̏���ۑ�����BoneTransform�^�̕ϐ�mLocalBindPose�ɕۑ����Ă���1
		temp.mLocalBindPose.mRotation.x = rot[0].GetDouble();
		temp.mLocalBindPose.mRotation.y = rot[1].GetDouble();
		temp.mLocalBindPose.mRotation.z = rot[2].GetDouble();
		temp.mLocalBindPose.mRotation.w = rot[3].GetDouble();

		temp.mLocalBindPose.mTranslation.x = trans[0].GetDouble();
		temp.mLocalBindPose.mTranslation.y = trans[1].GetDouble();
		temp.mLocalBindPose.mTranslation.z = trans[2].GetDouble();

		//�{�[���̔z��ɓo�^
		mBones.emplace_back(temp);
	}

	// Now that we have the bones
	//�s��v�Z�ŋt�o�C���h�|�[�Y�s����v�Z
	ComputeGlobalInvBindPose();

	return true;
}

void Skeleton::ComputeGlobalInvBindPose()
{
	// Resize to number of bones, which automatically fills identity
	//mGlobalInvBindPoses�̔z��̃T�C�Y���{�[���̐��ɕύX����
	mGlobalInvBindPoses.resize(GetNumBones());
	
	// Step 1: Compute global bind pose for each bone
	//�X�e�b�v1. �e�{�[���̃O���[�o���o�C���h�|�[�Y�s������߂�(���[�J�����W����O���[�o�����W�֕ϊ�����s��)
	
	// The global bind pose for root is just the local bind pose
	//���[�g�{�[�������A�O���[�o���o�C���h�s��ƃ��[�J���o�C���h�s��͓����Ȃ̂ŁA���̂܂ܑ������
	mGlobalInvBindPoses[0] = mBones[0].mLocalBindPose.ToMatrix();

	// Each remaining bone's global bind pose is its local pose
	// multiplied by the parent's global bind pose
	//�c��̃{�[���̓��[�J���|�[�Y�ɐe�̃O���[�o���|�[�Y������������
	for (size_t i = 1; i < mGlobalInvBindPoses.size(); i++)
	{
		//�ЂƂ܂��o�C���h�|�[�Y�s��(�O���[�o���|�[�Y�s��)�����߂�
		Matrix4 localMat = mBones[i].mLocalBindPose.ToMatrix();
		mGlobalInvBindPoses[i] = localMat * mGlobalInvBindPoses[mBones[i].mParent];
	}

	// Step 2: Invert
	//�X�e�b�v2. �e�s��̋t�s������߂�
	for (size_t i = 0; i < mGlobalInvBindPoses.size(); i++)
	{
		mGlobalInvBindPoses[i].Invert();
	}
}
