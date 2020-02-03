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

//���C�g�W�I���g�����g�p������@�ł́A�_�����̒��ɃJ����������ƃ��C�g�W�I���g����
//�`�悳��Ȃ����߁A���̌��ʂ����f����Ȃ��Ȃ�Ƃ������_������

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
	//���[���h�ϊ��́A�O���̔��a�ɂ���ăX�P�[�����O
	//���b�V���͒P�ʃT�C�Y�ɂȂ��Ă��Ȃ����߁A���b�V���̔��a�Ŋ���
	//(��������ƁAmOuterRadius��2�ŁA���b�V���w�i��2�̂Ƃ��́A�X�P�[�������l������Ηǂ����ƂɂȂ�)
	//�܂�A������mOuterRadius�̃T�C�Y�Ƀ��b�V����傫�����Ă���A�X�P�[�������傫�����邱�ƂɂȂ�
	Matrix4 scale = Matrix4::CreateScale(mOwner->GetScale() *
		mOuterRadius / mesh->GetRadius());
	//���s�ړ��͏����҂̈ʒu�Ɉˑ�
	Matrix4 trans = Matrix4::CreateTranslation(mOwner->GetPosition());
	//��]�͂Ȃ�(�������炵�Ă��Ӗ��Ȃ�)
	Matrix4 worldTransform = scale * trans;
	shader->SetMatrixUniform("uWorldTransform", worldTransform);
	// Set point light shader constants
	shader->SetVectorUniform("uPointLight.mWorldPos", mOwner->GetPosition());
	shader->SetVectorUniform("uPointLight.mDiffuseColor", mDiffuseColor);
	shader->SetFloatUniform("uPointLight.mInnerRadius", mInnerRadius);
	shader->SetFloatUniform("uPointLight.mOuterRadius", mOuterRadius);

	// Draw the sphere
	//���̃��b�V���̕`��
	//�`�悷��ƁA�_�������t���O�����g�ɗ^����F�̉e�����v�Z����A
	//���̐F���O���[�o�����C�e�B���O�̃p�X���`�����F�ɉ��Z�����
	glDrawElements(GL_TRIANGLES, mesh->GetVertexArray()->GetNumIndices(), 
		GL_UNSIGNED_INT, nullptr);
}
