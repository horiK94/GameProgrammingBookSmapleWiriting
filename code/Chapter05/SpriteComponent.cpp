// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "SpriteComponent.h"
#include "Texture.h"
#include "Shader.h"
#include "Actor.h"
#include "Game.h"

SpriteComponent::SpriteComponent(Actor* owner, int drawOrder)
	:Component(owner)
	,mTexture(nullptr)
	,mDrawOrder(drawOrder)
	,mTexWidth(0)
	,mTexHeight(0)
{
	mOwner->GetGame()->AddSprite(this);
}

SpriteComponent::~SpriteComponent()
{
	mOwner->GetGame()->RemoveSprite(this);
}

void SpriteComponent::Draw(Shader* shader)
{
	//if (mTexture)
	//{
	//	// Scale the quad by the width/height of texture
	//	Matrix4 scaleMat = Matrix4::CreateScale(
	//		static_cast<float>(mTexWidth),
	//		static_cast<float>(mTexHeight),
	//		1.0f);
	//	
	//	Matrix4 world = scaleMat * mOwner->GetWorldTransform();
	//	
	//	// Since all sprites use the same shader/vertices,
	//	// the game first sets them active before any sprite draws
	//	
	//	// Set world transform
	//	shader->SetMatrixUniform("uWorldTransform", world);
	//	// Set current texture
	//	mTexture->SetActive();
	//	// Draw quad
	//	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	//}
	//�e�N�X�`���̕��ƍ����̃X�P�[�����O�s��̍쐬
	Matrix4 scaleMat = Matrix4::CreateScale(
		static_cast<int>(mTexWidth),
		static_cast<int>(mTexHeight),
		1.0f);
	//�X�P�[�����O�s�񂩂烏�[���h��Ԃɂ��邽�߃��[���h�s����|����
	//���[���h��Ԃɂ��闝�R��140x140�̉摜�ɑ΂��āAactor��scale:2�Ƃ��Ă�����A280x280�ɂ��邽��
	Matrix4 world = scaleMat * mOwner->GetWorldTransform();

	//���[���h�ϊ��̐ݒ�
	shader->SetMatrixUniform("uWorldTransform", world);
	//�e�N�X�`�����A�N�e�B�u��
	mTexture->SetActive();
	//�X�v���C�g�̕`��(�`��O�ɒ��_�z��I�u�W�F�N�g�ƃV�F�[�_�[���A�N�e�B�u�ɂ���K�v����)
	glDrawElements(
		GL_TRIANGLES,			//�|���S���̎��
		6,						//�C���f�b�N�X�o�b�t�@�̂���ł������̐�
		GL_UNSIGNED_INT,		//�C���f�b�N�X�̌^
		nullptr);				//�ʏ�nullptr
}

void SpriteComponent::SetTexture(Texture* texture)
{
	mTexture = texture;
	// Set width/height
	mTexWidth = texture->GetWidth();
	mTexHeight = texture->GetHeight();
}
