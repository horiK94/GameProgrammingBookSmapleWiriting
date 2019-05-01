// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "SpriteComponent.h"
#include "Actor.h"
#include "Game.h"

SpriteComponent::SpriteComponent(Actor* owner, int drawOrder)
	:Component(owner)
	,mTexture(nullptr)
	,mDrawOrder(drawOrder)
	,mTexWidth(0)
	,mTexHeight(0)
{
	mOwner->GetGame()->AddSprite(this);		//���g��o�^����
}

SpriteComponent::~SpriteComponent()
{
	mOwner->GetGame()->RemoveSprite(this);
}

void SpriteComponent::Draw(SDL_Renderer* renderer)
{
	if (mTexture)
	{
		SDL_Rect r;		//SDL_Rext��x, y���W�͍���̍��W�̂��߁A�ȉ��ŕ␳���s��
		// Scale the width/height by owner's scale
		r.w = static_cast<int>(mTexWidth * mOwner->GetScale());		//����*�T�C�Y
		r.h = static_cast<int>(mTexHeight * mOwner->GetScale());		//�c��*�T�C�Y
		// Center the rectangle around the position of the owner
		r.x = static_cast<int>(mOwner->GetPosition().x - r.w / 2);		//Actor���ێ����Ă���mPosition�͒��S�_�̍��W�̂��߁A�C�����Ȃ���΂Ȃ�Ȃ�
		r.y = static_cast<int>(mOwner->GetPosition().y - r.h / 2);

		// Draw (have to convert angle from radians to degrees, and clockwise to counter)
		SDL_RenderCopyEx(renderer,		//�����_���[�^�[�Q�b�g
			mTexture,		//�`�悵�����e�N�X�`��	
			nullptr,		//�`�悵�����e�N�X�`���͈̔�(�S�̂�nullptr)
			&r,				//�`��͈͂̋�`(�v�́A�e�N�X�`���͂��̗̈�ɍ����悤�g��k�����s��)
			-Math::ToDegrees(mOwner->GetRotation()),		//��]�p(�x, ���v���)
			nullptr,		//��]���S(�����Ȃ�nullptr)
			SDL_FLIP_NONE);				//�e�N�X�`���𔽓]���邩(���]���Ȃ��ꍇ�͍��̂悤�ɐݒ肷��)
	}
}

//Texture��ݒ肷��
//Texture...�摜���̂���
//Sprite..2D�O���t�B�J���I�u�W�F�N�g. Texturen�̂����̕������g�p���܂���Ƃ��������Ƃ��w�肷��N���X
//Atlas����R�}����A�j���Ƃ��̂Ƃ��́ATexture != Sprite�̎g�p�����ƂȂ鎖������
void SpriteComponent::SetTexture(SDL_Texture* texture)
{
	mTexture = texture;
	// Set width/height
	SDL_QueryTexture(texture, nullptr, nullptr, &mTexWidth, &mTexHeight);		//Texture�̏��̎擾
	//�ȒP�Ɍ�����, ��P������Texture��������ƁAmTexWidth, mTexHeight���Ԃ����

	//Texture�̕`��͈ȉ���2���
	/*
	�V���v��:
	int SDL_RenderCopy(SDL_Renderer* renderer, SDL_Texture* texture, const SDL_Rect* srcrect, const SDL_Rect* dstrect)
	SDL_Renderer* renderer:�`�悷�郌���_�����O�R���e�L�X�g
	SDL_Texture* texture: �`�悵�����e�N�X�`��
	const SDL_Rect* srcrect: �`�悵�����e�N�X�`���͈̔�(���ׂĂȂ�nullptr)
	const SDL_Rect* dstrect: �`��͈͂̋�`(�v�́A�e�N�X�`���͂��̗̈�ɍ����悤�g��k�����s��)
	
	�X�v���C�g����]���������ꍇ�͈ȉ��̊֐����g�p����

	*/
}
