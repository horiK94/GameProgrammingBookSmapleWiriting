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

	//file情報をstringstreamに変換し、中身をstringに移したら、jsonのstringに変換
	std::stringstream fileStream;
	fileStream << file.rdbuf();
	std::string contents = fileStream.str();
	rapidjson::StringStream jsonStr(contents.c_str());
	rapidjson::Document doc;
	//jsonStrをDOM Treeの形に変換している
	doc.ParseStream(jsonStr);

	if (!doc.IsObject())
	{
		//DOM Treeの形にできなかった
		SDL_Log("Skeleton %s is not valid json", fileName.c_str());
		return false;
	}

	int ver = doc["version"].GetInt();

	// Check the metadata
	if (ver != 1)
	{
		//versionが1じゃなかった
		SDL_Log("Skeleton %s unknown format", fileName.c_str());
		return false;
	}

	const rapidjson::Value& bonecount = doc["bonecount"];
	if (!bonecount.IsUint())
	{
		//boneの数(bonecount)がuintではなかった
		SDL_Log("Skeleton %s doesn't have a bone count.", fileName.c_str());
		return false;
	}

	size_t count = bonecount.GetUint();

	if (count > MAX_SKELETON_BONES)
	{
		//ボーン数が一定の値を超えている場合
		SDL_Log("Skeleton %s exceeds maximum bone count.", fileName.c_str());
		return false;
	}

	// mBones配列をcountのサイズに再構築
	mBones.reserve(count);

	const rapidjson::Value& bones = doc["bones"];
	if (!bones.IsArray())
	{
		//bonesが配列でない
		SDL_Log("Skeleton %s doesn't have a bone array?", fileName.c_str());
		return false;
	}

	if (bones.Size() != count)
	{
		//bones配列のサイズがcountと一致しない
		SDL_Log("Skeleton %s has a mismatch between the bone count and number of bones", fileName.c_str());
		return false;
	}

	Bone temp;

	//ボーンの数だけ探索する
	for (rapidjson::SizeType i = 0; i < count; i++)
	{
		if (!bones[i].IsObject())
		{
			//bonesの中がDOM treeにできなかったとき
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
			//bindposeの中がDOM Treeにできなかったとき
			SDL_Log("Skeleton %s: Bone %d is invalid.", fileName.c_str(), i);
			return false;
		}

		const rapidjson::Value& rot = bindpose["rot"];
		const rapidjson::Value& trans = bindpose["trans"];

		if (!rot.IsArray() || !trans.IsArray())
		{
			//rotもtransも配列であるという状態ではなかったとき
			SDL_Log("Skeleton %s: Bone %d is invalid.", fileName.c_str(), i);
			return false;
		}

		//回転や位置の情報を保存するBoneTransform型の変数mLocalBindPoseに保存していく1
		temp.mLocalBindPose.mRotation.x = rot[0].GetDouble();
		temp.mLocalBindPose.mRotation.y = rot[1].GetDouble();
		temp.mLocalBindPose.mRotation.z = rot[2].GetDouble();
		temp.mLocalBindPose.mRotation.w = rot[3].GetDouble();

		temp.mLocalBindPose.mTranslation.x = trans[0].GetDouble();
		temp.mLocalBindPose.mTranslation.y = trans[1].GetDouble();
		temp.mLocalBindPose.mTranslation.z = trans[2].GetDouble();

		//ボーンの配列に登録
		mBones.emplace_back(temp);
	}

	// Now that we have the bones
	//行列計算で逆バインドポーズ行列を計算
	ComputeGlobalInvBindPose();

	return true;
}

void Skeleton::ComputeGlobalInvBindPose()
{
	// Resize to number of bones, which automatically fills identity
	//mGlobalInvBindPosesの配列のサイズをボーンの数に変更する
	mGlobalInvBindPoses.resize(GetNumBones());
	
	// Step 1: Compute global bind pose for each bone
	//ステップ1. 各ボーンのグローバルバインドポーズ行列を求める(ローカル座標からグローバル座標へ変換する行列)
	
	// The global bind pose for root is just the local bind pose
	//ルートボーンだけ、グローバルバインド行列とローカルバインド行列は同じなので、そのまま代入する
	mGlobalInvBindPoses[0] = mBones[0].mLocalBindPose.ToMatrix();

	// Each remaining bone's global bind pose is its local pose
	// multiplied by the parent's global bind pose
	//残りのボーンはローカルポーズに親のグローバルポーズをかけたもの
	for (size_t i = 1; i < mGlobalInvBindPoses.size(); i++)
	{
		//ひとまずバインドポーズ行列(グローバルポーズ行列)を求める
		Matrix4 localMat = mBones[i].mLocalBindPose.ToMatrix();
		mGlobalInvBindPoses[i] = localMat * mGlobalInvBindPoses[mBones[i].mParent];
	}

	// Step 2: Invert
	//ステップ2. 各行列の逆行列を求める
	for (size_t i = 0; i < mGlobalInvBindPoses.size(); i++)
	{
		mGlobalInvBindPoses[i].Invert();
	}
}
