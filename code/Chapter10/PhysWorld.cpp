// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "PhysWorld.h"
#include <algorithm>
#include "BoxComponent.h"
#include <SDL/SDL.h>
#include <vector>
#include <map>
#include <list>

PhysWorld::PhysWorld(Game* game)
	:mGame(game)
{
}

bool PhysWorld::SegmentCast(const LineSegment& l, CollisionInfo& outColl)
{
	//bool collided = false;
	//// Initialize closestT to infinity, so first
	//// intersection will always update closestT
	//float closestT = Math::Infinity;
	//Vector3 norm;
	//// Test against all boxes
	//for (auto box : mBoxes)
	//{
	//	float t;
	//	// Does the segment intersect with the box?
	//	if (Intersect(l, box->GetWorldBox(), t, norm))
	//	{
	//		// Is this closer than previous intersection?
	//		if (t < closestT)
	//		{
	//			outColl.mPoint = l.PointOnSegment(t);
	//			outColl.mNormal = norm;
	//			outColl.mBox = box;
	//			outColl.mActor = box->GetOwner();
	//			collided = true;
	//		}
	//	}
	//}
	//return collided;

	bool mColllied = false;
	//衝突したときのtの値を∞で設定する(1つでもボックスに衝突すれば、その衝突時の値の小さいものが代入されていく)
	float closestT = Math::Infinity;
	Vector3 norm;		//法線ベクトル

	//全ボックスをテスト
	for (auto box : mBoxes)
	{
		float t;
		//線分とボックスが交差するか
		if (Intersect(l, box->GetWorldBox(), t, norm))
		{
			if (t < closestT)
			{
				closestT = t;
				outColl.mPoint = l.PointOnSegment(t);
				outColl.mNormal = norm;
				outColl.mBox = box;
				outColl.mActor = box->GetOwner();
				mColllied = true;
			}
		}
	}
	return mColllied;
}

void PhysWorld::TestPairwise(std::function<void(Actor*, Actor*)> f)
{
	//// Naive implementation O(n^2)
	//for (size_t i = 0; i < mBoxes.size(); i++)
	//{
	//	// Don't need to test vs itself and any previous i values
	//	for (size_t j = i + 1; j < mBoxes.size(); j++)
	//	{
	//		BoxComponent* a = mBoxes[i];
	//		BoxComponent* b = mBoxes[j];
	//		if (Intersect(a->GetWorldBox(), b->GetWorldBox()))
	//		{
	//			// Call supplied function to handle intersection
	//			f(a->GetOwner(), b->GetOwner());
	//		}
	//	}
	//}

	//素直な実装 O(n^2)
	for (size_t i = 0; i < mBoxes.size(); i++)
	{
		for (size_t k = i + 1; k < mBoxes.size(); k++)
		{
			BoxComponent* a = mBoxes[i];
			BoxComponent* b = mBoxes[k];
			if (Intersect(a->GetWorldBox(), b->GetWorldBox()))
			{
				//衝突があった場合
				//提供された関数を呼び出す
				f(a->GetOwner(), b->GetOwner());
			}
		}
	}
}

void PhysWorld::TestSweepAndPrune(std::function<void(Actor*, Actor*)> f)
{
	// Sort by min.x
	//std::sort(mBoxes.begin(), mBoxes.end(),
	//	[](BoxComponent* a, BoxComponent* b) {
	//		return a->GetWorldBox().mMin.x <
	//			b->GetWorldBox().mMin.x;
	//	});

	//for (size_t i = 0; i < mBoxes.size(); i++)
	//{
	//	// Get max.x for current box
	//	BoxComponent* a = mBoxes[i];
	//	float max = a->GetWorldBox().mMax.x;
	//	for (size_t j = i + 1; j < mBoxes.size(); j++)
	//	{
	//		BoxComponent* b = mBoxes[j];
	//		// If AABB[j] min is past the max bounds of AABB[i],
	//		// then there aren't any other possible intersections
	//		// against AABB[i]
	//		if (b->GetWorldBox().mMin.x > max)
	//		{
	//			break;
	//		}
	//		else if (Intersect(a->GetWorldBox(), b->GetWorldBox()))
	//		{
	//			f(a->GetOwner(), b->GetOwner());
	//		}
	//	}
	//}

	//min.xですべてのバウンディングボックスを並び替え
	//std::sort(mBoxes.begin(), mBoxes.end(),
	//	[](BoxComponent* a, BoxComponent* b)
	//	{
	//		return a->GetWorldBox().mMin.x < b->GetWorldBox().mMin.x;
	//	});
	//for (size_t i = 0; i < mBoxes.size(); i++)
	//{
	//	//box[i]のBoxComponent取得
	//	BoxComponent* a = mBoxes[i];
	//	float max = a->GetWorldBox().mMax.x;
	//	for (size_t k = i + 1; k < mBoxes.size(); k++)
	//	{
	//		BoxComponent* b = mBoxes[k];
	//		if (b->GetWorldBox().mMin.x > max)
	//		{
	//			//box[k]のmin.xの値がbox[i]のmax.xの値より大きかったら、box[i]と交差する可能性があるボックスは他に存在しない
	//			break;
	//		}
	//		//交差する可能性があるボックスがある
	//		if (Intersect(a->GetWorldBox(), b->GetWorldBox()))
	//		{
	//			f(a->GetOwner(), b->GetOwner());
	//		}
	//	}
	//}

	//ボックスをminXが小さい順に並べる
	std::sort(mBoxes.begin(), mBoxes.end(),
		[](BoxComponent* a, BoxComponent* b)
		{
			return a->GetWorldBox().mMin.x < b->GetWorldBox().mMin.x;
		});

	//x座標における交差するボックスペア
	std::map<BoxComponent*, std::list<BoxComponent*>> collisionX;
	for (size_t i = 0; i < mBoxes.size(); i++)
	{
		BoxComponent* a = mBoxes[i];
		float max = a->GetWorldBox().mMax.x;
		for (size_t k = i + 1; mBoxes.size(); k++)
		{
			BoxComponent* b = mBoxes[k];
			if (b->GetWorldBox().mMin.x > max)
			{
				return;
			}
			collisionX[a].push_back(b);
		}
	}
}

void PhysWorld::AddBox(BoxComponent* box)
{
	mBoxes.emplace_back(box);
	boxesX.emplace_back(box);
	boxesY.emplace_back(box);
	boxesZ.emplace_back(box);
}

void PhysWorld::RemoveBox(BoxComponent* box)
{
	auto iter = std::find(mBoxes.begin(), mBoxes.end(), box);
	if (iter != mBoxes.end())
	{
		// Swap to end of vector and pop off (avoid erase copies)
		std::iter_swap(iter, mBoxes.end() - 1);
		mBoxes.pop_back();
	}
	auto boxX = std::find(boxesX.begin(), boxesX.end(), box);
	if (boxX != boxesX.end())
	{
		std::iter_swap(boxX, boxesX.end() - 1);
	}
	auto boxY = std::find(boxesY.begin(), boxesY.end(), box);
	if (boxY != boxesY.end())
	{
		std::iter_swap(boxY, boxesY.end() - 1);
	}
	auto boxZ = std::find(boxesZ.begin(), boxesZ.end(), box);
	if (boxZ != boxesZ.end())
	{
		std::iter_swap(boxZ, boxesZ.end() - 1);
	}
}
