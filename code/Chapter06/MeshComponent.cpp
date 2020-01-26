// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "MeshComponent.h"
#include "Shader.h"
#include "Mesh.h"
#include "Actor.h"
#include "Game.h"
#include "Renderer.h"
#include "Texture.h"
#include "VertexArray.h"

MeshComponent::MeshComponent(Actor* owner)
	:Component(owner)
	,mMesh(nullptr)
	,mTextureIndex(0)
{
	//mOwner->GetGame()->GetRenderer()->AddMeshComp(this);
}

MeshComponent::~MeshComponent()
{
	mOwner->GetGame()->GetRenderer()->RemoveMeshComp(this);
}

void MeshComponent::Draw(Shader* shader)
{
	if (mMesh)
	{
		//ワールド座標の設定
		//SpriteComponentと違い、
		shader->SetMatrixUniform("uWorldTransform", mOwner->GetWorldTransform());
		//鏡面反射指数の指定
		shader->SetFloatUniform("uSpecPower", mMesh->GetSpecPower());
		//使用するテクスチャの取得
		Texture* t = mMesh->GetTexture(mTextureIndex);
		if (t)
		{
			//テクスチャをアクティブに
			t->SetActive();
		}
		//頂点情報を取得
		VertexArray* va = mMesh->GetVertexArray();
		va->SetActive();
		//描画する
		glDrawElements(GL_TRIANGLES,		//ポリゴンは三角形 
			va->GetNumIndices(),		//インデックスバッファの数
			GL_UNSIGNED_INT,		//符号なしint
			nullptr);
	}
	//if (mMesh)
	//{
	//	// Set the world transform
	//	shader->SetMatrixUniform("uWorldTransform", 
	//		mOwner->GetWorldTransform());
	//	// Set specular power
	//	shader->SetFloatUniform("uSpecPower", mMesh->GetSpecPower());
	//	// Set the active texture
	//	Texture* t = mMesh->GetTexture(mTextureIndex);
	//	if (t)
	//	{
	//		t->SetActive();
	//	}
	//	// Set the mesh's vertex array as active
	//	VertexArray* va = mMesh->GetVertexArray();
	//	va->SetActive();
	//	// Draw
	//	glDrawElements(GL_TRIANGLES, va->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
	//}
}

void MeshComponent::SetMesh(Mesh* mesh)
{
	mMesh = mesh;
	mOwner->GetGame()->GetRenderer()->AddMeshComp(mMesh->GetShaderName(), this);
}
