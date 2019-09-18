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
		//���[���h���W�̐ݒ�
		//SpriteComponent�ƈႢ�A
		shader->SetMatrixUniform("uWorldTransform", mOwner->GetWorldTransform());
		//���ʔ��ˎw���̎w��
		shader->SetFloatUniform("uSpecPower", mMesh->GetSpecPower());
		//�g�p����e�N�X�`���̎擾
		Texture* t = mMesh->GetTexture(mTextureIndex);
		if (t)
		{
			//�e�N�X�`�����A�N�e�B�u��
			t->SetActive();
		}
		//���_�����擾
		VertexArray* va = mMesh->GetVertexArray();
		va->SetActive();
		//�`�悷��
		glDrawElements(GL_TRIANGLES,		//�|���S���͎O�p�` 
			va->GetNumIndices(),		//�C���f�b�N�X�o�b�t�@�̐�
			GL_UNSIGNED_INT,		//�����Ȃ�int
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
