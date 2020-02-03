// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "Math.h"
#include "Component.h"

class PointLightComponent : public Component
{
public:
	PointLightComponent(class Actor* owner);
	~PointLightComponent();

	// Draw this point light as geometry
	//点光源をジオメトリとして描画
	void Draw(class Shader* shader, class Mesh* mesh);

	// Diffuse color
	//拡散色
	Vector3 mDiffuseColor;
	// Radius of light
	//内側の半径(これより内側は完全な拡散色が反映される)
	float mInnerRadius;
	//外側の背景(これより外側は点光源に影響されない)
	float mOuterRadius;
};
