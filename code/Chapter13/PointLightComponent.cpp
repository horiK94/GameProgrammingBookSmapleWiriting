// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "PointLightComponent.h"
#include "Shader.h"
#include "Game.h"
#include "Renderer.h"
#include "Mesh.h"
#include "VertexArray.h"
#include "Actor.h"

//ライトジオメトリを使用する方法では、点光源の中にカメラが入るとライトジオメトリが
//描画されないため、光の効果が反映されなくなるという問題点もある

PointLightComponent::PointLightComponent(Actor* owner)
	:Component(owner)
{
	owner->GetGame()->GetRenderer()->AddPointLight(this);
}

PointLightComponent::~PointLightComponent()
{
	mOwner->GetGame()->GetRenderer()->RemovePointLight(this);
}

void PointLightComponent::Draw(Shader* shader, Mesh* mesh)
{
	// We assume, coming into this function, that the shader is active
	// and the sphere mesh is active

	// World transform is scaled to the outer radius (divided by the mesh radius)
	// and positioned to the world position
	//ワールド変換は、外側の半径によってスケーリング
	//メッシュは単位サイズになっていないため、メッシュの半径で割る
	//(そうすると、mOuterRadiusが2で、メッシュ背景も2のときは、スケールだけ考慮すれば良いことになる)
	//つまり、正しいmOuterRadiusのサイズにメッシュを大きくしてから、スケールだけ大きくすることになる
	Matrix4 scale = Matrix4::CreateScale(mOwner->GetScale() *
		mOuterRadius / mesh->GetRadius());
	//平行移動は所持者の位置に依存
	Matrix4 trans = Matrix4::CreateTranslation(mOwner->GetPosition());
	//回転はなし(球だからしても意味ない)
	Matrix4 worldTransform = scale * trans;
	shader->SetMatrixUniform("uWorldTransform", worldTransform);
	// Set point light shader constants
	shader->SetVectorUniform("uPointLight.mWorldPos", mOwner->GetPosition());
	shader->SetVectorUniform("uPointLight.mDiffuseColor", mDiffuseColor);
	shader->SetFloatUniform("uPointLight.mInnerRadius", mInnerRadius);
	shader->SetFloatUniform("uPointLight.mOuterRadius", mOuterRadius);

	// Draw the sphere
	//球のメッシュの描画
	//描画すると、点光源がフラグメントに与える色の影響が計算され、
	//その色がグローバルライティングのパスが描いた色に加算される
	glDrawElements(GL_TRIANGLES, mesh->GetVertexArray()->GetNumIndices(), 
		GL_UNSIGNED_INT, nullptr);
}
