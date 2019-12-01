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
	//十字の表示
	//Update()で衝突しているEnemyがあったか調べ終わっているので、結果をもとにテクスチャを変更
	Texture* cross = mTargetEnemy ? mCrosshairEnemy : mCrosshair;
	DrawTexture(shader, cross, Vector2::Zero, 2.0f);
	
	// Radar
	//レーダーの背景表示
	const Vector2 cRadarPos(-390.0f, 275.0f);
	DrawTexture(shader, mRadar, cRadarPos, 1.0f);
	// Blips
	//輝点の表示
	for (Vector2& blip : mBlips)
	{
		//各輝点の位置をレーダーの背景画像の中心を基に描画
		DrawTexture(shader, mBlipTex, cRadarPos + blip, 1.0f);
	}
	// Radar arrow
	//レーダー内の自身のアイコン表示
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
	//当たり判定の情報をリセット
	mTargetEnemy = false;
	// Make a line segment
	//線分の作成
	const float cAimDist = 5000.0f;
	Vector3 start, dir;
	//画面の中心点に向けてカメラからrayを飛ばしたときの近接平面上の点(=start)と、
	//近接平面上の点から遠方平面上の点までのワールド空間における方向ベクトルを取得する
	mGame->GetRenderer()->GetScreenDirection(start, dir);
	LineSegment l(start, start + dir * cAimDist);
	// Segment cast
	PhysWorld::CollisionInfo info;
	if (mGame->GetPhysWorld()->SegmentCast(l, info))
	{
		//BoxComponentをアタッチしている物体と衝突したとき
		// Is this a target?
		for (auto tc : mTargetComps)
		{
			if (tc->GetOwner() == info.mActor)
			{
				//ターゲットコンポーネントを所持しているアクターが衝突したアクターだったらターゲットフラグと立てる
				mTargetEnemy = true;
				break;
			}
		}
	}
}

void HUD::UpdateRadar(float deltaTime)
{
	// Clear blip positions from last frame
	//輝点の位置をクリア
	mBlips.clear();
	
	// Convert player position to radar coordinates (x forward, z up)
	//プレイヤーの位置をレーダー座標に変換(x, y, z) => (y, x)に変換(位置で使用している座標系とと計算で使用している座標系が異なるため)
	Vector3 playerPos = mGame->GetPlayer()->GetPosition();
	Vector2 playerPos2D(playerPos.y, playerPos.x);
	// Ditto for player forward
	//プレイヤーの前方ベクトルの取得(初期から動かなければ(1, 0)となる)
	Vector3 playerForward = mGame->GetPlayer()->GetForward();
	Vector2 playerForward2D(playerForward.x, playerForward.y);
	
	// Use atan2 to get rotation of radar
	//なす角θをatan2で求める(参考: https://cpprefjp.github.io/reference/cmath/atan2.html )[-pi, pi]で結果が帰ってくる
	//初期から動かなければ(0, 1)となるからangle = 0となる
	float angle = Math::Atan2(playerForward2D.y, playerForward2D.x);
	// Make a 2D rotation matrix
	//求めたθだけ2D回転を行う回転行列を求める
	Matrix3 rotMat = Matrix3::CreateRotation(angle);
	
	// Get positions of blips
	//輝点の位置の取得
	for (auto tc : mTargetComps)
	{
		//全ターゲットコンポーネントに対して処理を行う

		//ターゲットの位置取得
		Vector3 targetPos = tc->GetOwner()->GetPosition();
		//(x, y, z) => (y, x)に変換(位置で使用している座標系とと計算で使用している座標系が異なるため)
		Vector2 actorPos2D(targetPos.y, targetPos.x);
		
		// Calculate vector between player and target
		//プレイヤーからみたターゲットの相対ベクトルを求める
		Vector2 playerToTarget = actorPos2D - playerPos2D;
		
		// See if within range
		//プレイヤーから見たターゲットまでの距離の2乗が, ワールド座標におけるレーダーの検知可能範囲の2乗以内かチェック
		if (playerToTarget.LengthSq() <= (mRadarRange * mRadarRange))
		{
			// Convert playerToTarget into an offset from
			// the center of the on-screen radar
			Vector2 blipPos = playerToTarget;
			//playerToTargetを画面上のレーダーの中心からのオフセットに変換
			blipPos *= mRadarRadius/mRadarRange;
			
			// Rotate blipPos
			//プレイヤーの回転座標をもとに位置を回転
			blipPos = Vector2::Transform(blipPos, rotMat);
			//保存
			mBlips.emplace_back(blipPos);
		}
	}
}
