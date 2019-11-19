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

//�����N���X(���̂Ƃ���A�R���W�����܂Ƃ߂��N���X�ƌ�������)
class PhysWorld
{
public:
	PhysWorld(class Game* game);

	// Used to give helpful information about collision results
	//�Փˎ��ɕԂ��R���W�������
	struct CollisionInfo
	{
		// Point of collision
		//�Փˈʒu
		Vector3 mPoint;
		// Normal at collision
		//�Փ˓X�̖@��
		Vector3 mNormal;
		// Component collided with
		//��������BoxComponent
		class BoxComponent* mBox;
		// Owning actor of componnet
		//��������Component�����L����Actor
		class Actor* mActor;
	};

	// Test a line segment against boxes
	// Returns true if it collides against a box
	//�������󂯎��A�����ƃ{�b�N�X����������Ȃ�true�ŕԂ��B����ɕ����������邱�Ƃ��z�肵�A�ŏ��Ɍ�������Collision����Ԃ�
	bool SegmentCast(const LineSegment& l, CollisionInfo& outColl);

	// Tests collisions using naive pairwise
	void TestPairwise(std::function<void(class Actor*, class Actor*)> f);
	// Test collisions using sweep and prune
	void TestSweepAndPrune(std::function<void(class Actor*, class Actor*)> f);

	// Add/remove box components from world
	void AddBox(class BoxComponent* box);		//BoxComponent�ǉ�
	void RemoveBox(class BoxComponent* box);		//��菜��
private:
	class Game* mGame;
	std::vector<class BoxComponent*> mBoxes;		//BoxComponent�̎Q�Ɣz��
};
