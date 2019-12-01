// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "HUD.h"
#include "Texture.h"
#include "Shader.h"
#include "Game.h"
#include "Renderer.h"
#include "PhysWorld.h"
#include "FPSActor.h"
#include <algorithm>
#include "TargetComponent.h"

HUD::HUD(Game* game)
	:UIScreen(game)
	,mRadarRange(200.0f)
	,mRadarRadius(192.0f)
	,mTargetEnemy(false)
{
	Renderer* r = mGame->GetRenderer();
	mHealthBar = r->GetTexture("Assets/HealthBar.png");
	mRadar = r->GetTexture("Assets/Radar.png");
	mCrosshair = r->GetTexture("Assets/Crosshair.png");
	mCrosshairEnemy = r->GetTexture("Assets/CrosshairRed.png");
	mBlipTex = r->GetTexture("Assets/Blip.png");
	mRadarArrow = r->GetTexture("Assets/RadarArrow.png");
}

HUD::~HUD()
{
}

void HUD::Update(float deltaTime)
{
	UIScreen::Update(deltaTime);
	
	UpdateCrosshair(deltaTime);
	UpdateRadar(deltaTime);
}

void HUD::Draw(Shader* shader)
{
	// Crosshair
	//�\���̕\��
	//Update()�ŏՓ˂��Ă���Enemy�������������׏I����Ă���̂ŁA���ʂ����ƂɃe�N�X�`����ύX
	Texture* cross = mTargetEnemy ? mCrosshairEnemy : mCrosshair;
	DrawTexture(shader, cross, Vector2::Zero, 2.0f);
	
	// Radar
	//���[�_�[�̔w�i�\��
	const Vector2 cRadarPos(-390.0f, 275.0f);
	DrawTexture(shader, mRadar, cRadarPos, 1.0f);
	// Blips
	//�P�_�̕\��
	for (Vector2& blip : mBlips)
	{
		//�e�P�_�̈ʒu�����[�_�[�̔w�i�摜�̒��S����ɕ`��
		DrawTexture(shader, mBlipTex, cRadarPos + blip, 1.0f);
	}
	// Radar arrow
	//���[�_�[���̎��g�̃A�C�R���\��
	DrawTexture(shader, mRadarArrow, cRadarPos);
	
	// Health bar
	DrawTexture(shader, mHealthBar, Vector2(-350.0f, -350.0f));
}

void HUD::AddTargetComponent(TargetComponent* tc)
{
	mTargetComps.emplace_back(tc);
}

void HUD::RemoveTargetComponent(TargetComponent* tc)
{
	auto iter = std::find(mTargetComps.begin(), mTargetComps.end(),
		tc);
	mTargetComps.erase(iter);
}

void HUD::UpdateCrosshair(float deltaTime)
{
	// Reset to regular cursor
	//�����蔻��̏������Z�b�g
	mTargetEnemy = false;
	// Make a line segment
	//�����̍쐬
	const float cAimDist = 5000.0f;
	Vector3 start, dir;
	//��ʂ̒��S�_�Ɍ����ăJ��������ray���΂����Ƃ��̋ߐڕ��ʏ�̓_(=start)�ƁA
	//�ߐڕ��ʏ�̓_���牓�����ʏ�̓_�܂ł̃��[���h��Ԃɂ���������x�N�g�����擾����
	mGame->GetRenderer()->GetScreenDirection(start, dir);
	LineSegment l(start, start + dir * cAimDist);
	// Segment cast
	PhysWorld::CollisionInfo info;
	if (mGame->GetPhysWorld()->SegmentCast(l, info))
	{
		//BoxComponent���A�^�b�`���Ă��镨�̂ƏՓ˂����Ƃ�
		// Is this a target?
		for (auto tc : mTargetComps)
		{
			if (tc->GetOwner() == info.mActor)
			{
				//�^�[�Q�b�g�R���|�[�l���g���������Ă���A�N�^�[���Փ˂����A�N�^�[��������^�[�Q�b�g�t���O�Ɨ��Ă�
				mTargetEnemy = true;
				break;
			}
		}
	}
}

void HUD::UpdateRadar(float deltaTime)
{
	// Clear blip positions from last frame
	//�P�_�̈ʒu���N���A
	mBlips.clear();
	
	// Convert player position to radar coordinates (x forward, z up)
	//�v���C���[�̈ʒu�����[�_�[���W�ɕϊ�(x, y, z) => (y, x)�ɕϊ�(�ʒu�Ŏg�p���Ă�����W�n�Ƃƌv�Z�Ŏg�p���Ă�����W�n���قȂ邽��)
	Vector3 playerPos = mGame->GetPlayer()->GetPosition();
	Vector2 playerPos2D(playerPos.y, playerPos.x);
	// Ditto for player forward
	//�v���C���[�̑O���x�N�g���̎擾(�������瓮���Ȃ����(1, 0)�ƂȂ�)
	Vector3 playerForward = mGame->GetPlayer()->GetForward();
	Vector2 playerForward2D(playerForward.x, playerForward.y);
	
	// Use atan2 to get rotation of radar
	//�Ȃ��p�Ƃ�atan2�ŋ��߂�(�Q�l: https://cpprefjp.github.io/reference/cmath/atan2.html )[-pi, pi]�Ō��ʂ��A���Ă���
	//�������瓮���Ȃ����(0, 1)�ƂȂ邩��angle = 0�ƂȂ�
	float angle = Math::Atan2(playerForward2D.y, playerForward2D.x);
	// Make a 2D rotation matrix
	//���߂��Ƃ���2D��]���s����]�s������߂�
	Matrix3 rotMat = Matrix3::CreateRotation(angle);
	
	// Get positions of blips
	//�P�_�̈ʒu�̎擾
	for (auto tc : mTargetComps)
	{
		//�S�^�[�Q�b�g�R���|�[�l���g�ɑ΂��ď������s��

		//�^�[�Q�b�g�̈ʒu�擾
		Vector3 targetPos = tc->GetOwner()->GetPosition();
		//(x, y, z) => (y, x)�ɕϊ�(�ʒu�Ŏg�p���Ă�����W�n�Ƃƌv�Z�Ŏg�p���Ă�����W�n���قȂ邽��)
		Vector2 actorPos2D(targetPos.y, targetPos.x);
		
		// Calculate vector between player and target
		//�v���C���[����݂��^�[�Q�b�g�̑��΃x�N�g�������߂�
		Vector2 playerToTarget = actorPos2D - playerPos2D;
		
		// See if within range
		//�v���C���[���猩���^�[�Q�b�g�܂ł̋�����2�悪, ���[���h���W�ɂ����郌�[�_�[�̌��m�\�͈͂�2��ȓ����`�F�b�N
		if (playerToTarget.LengthSq() <= (mRadarRange * mRadarRange))
		{
			// Convert playerToTarget into an offset from
			// the center of the on-screen radar
			Vector2 blipPos = playerToTarget;
			//playerToTarget����ʏ�̃��[�_�[�̒��S����̃I�t�Z�b�g�ɕϊ�
			blipPos *= mRadarRadius/mRadarRange;
			
			// Rotate blipPos
			//�v���C���[�̉�]���W�����ƂɈʒu����]
			blipPos = Vector2::Transform(blipPos, rotMat);
			//�ۑ�
			mBlips.emplace_back(blipPos);
		}
	}
}
