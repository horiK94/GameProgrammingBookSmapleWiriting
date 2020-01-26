// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "UIScreen.h"
#include <vector>

class HUD : public UIScreen
{
public:
	// (Lower draw order corresponds with further back)
	HUD(class Game* game);
	~HUD();

	void Update(float deltaTime) override;
	void Draw(class Shader* shader) override;
	
	//TargetComponentの追加
	void AddTargetComponent(class TargetComponent* tc);
	void RemoveTargetComponent(class TargetComponent* tc);
protected:
	//衝突しているかどうかチェックする関数
	void UpdateCrosshair(float deltaTime);
	//レーザーの輝点の更新
	void UpdateRadar(float deltaTime);
	
	class Texture* mHealthBar;
	class Texture* mRadar;
	//十字線のテクスチャ
	class Texture* mCrosshair;
	//敵を捉えたときの十字線のテクスチャ
	class Texture* mCrosshairEnemy;
	class Texture* mBlipTex;
	class Texture* mRadarArrow;
	
	// All the target components in the game
	// ターゲット把握コンポーネントのポインタ配列
	std::vector<class TargetComponent*> mTargetComps;
	// 2D offsets of blips relative to radar
	//レーダー中心から輝点への2D相対オフセット
	//レーダーの更新時はここの値を更新する
	//背景描画後、この値をもとに輝点を描画すれば良い
	std::vector<Vector2> mBlips;
	// Adjust range of radar and radius
	//レーダーの表示有効範囲(この範囲ギリギリのときは、レーダーの輝点が消えるギリギリの遠さにあるということ)(上から見たワールド座標のサイズ)
	float mRadarRange;
	//レーダーの輝点の表示上での有効半径(画像サイズに合わせる必要がある)(スクリーン2Dのサイズ)
	float mRadarRadius;
	// Whether the crosshair targets an enemy
	//十字線が敵を捉えているか
	bool mTargetEnemy;
};
