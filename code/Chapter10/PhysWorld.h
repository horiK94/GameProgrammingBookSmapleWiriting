// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include <vector>
#include <functional>
#include "Math.h"
#include "Collision.h"

//物理クラス(今のところ、コリジョンまとめたクラスと言う感じ)
class PhysWorld
{
public:
	PhysWorld(class Game* game);

	// Used to give helpful information about collision results
	//衝突時に返すコリジョン情報
	struct CollisionInfo
	{
		// Point of collision
		//衝突位置
		Vector3 mPoint;
		// Normal at collision
		//衝突店の法線
		Vector3 mNormal;
		// Component collided with
		//交差したBoxComponent
		class BoxComponent* mBox;
		// Owning actor of componnet
		//交差したComponentを所有するActor
		class Actor* mActor;
	};

	// Test a line segment against boxes
	// Returns true if it collides against a box
	//線分を受け取り、線分とボックスが交差するならtrueで返す。さらに複数個交差することも想定し、最初に交差したCollision情報を返す
	bool SegmentCast(const LineSegment& l, CollisionInfo& outColl);

	// Tests collisions using naive pairwise
	void TestPairwise(std::function<void(class Actor*, class Actor*)> f);
	// Test collisions using sweep and prune
	void TestSweepAndPrune(std::function<void(class Actor*, class Actor*)> f);

	// Add/remove box components from world
	void AddBox(class BoxComponent* box);		//BoxComponent追加
	void RemoveBox(class BoxComponent* box);		//取り除く
private:
	class Game* mGame;
	std::vector<class BoxComponent*> mBoxes;		//BoxComponentの参照配列
};
