// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Texture.h"
#include <SOIL/SOIL.h>
#include <GL/glew.h>
#include <SDL/SDL.h>

Texture::Texture()
:mTextureID(0)
,mWidth(0)
,mHeight(0)
{
	
}

Texture::~Texture()
{
	
}

bool Texture::Load(const std::string& fileName)
{
	//int channels = 0;
	
	//unsigned char* image = SOIL_load_image(fileName.c_str(),
	//									   &mWidth, &mHeight, &channels, SOIL_LOAD_AUTO);
	int channels = 0;
	unsigned char* image = SOIL_load_image(
		fileName.c_str(),
		&mWidth,		//�����L�^�����
		&mHeight,
		&channels,
		SOIL_LOAD_AUTO		//�摜�t�@�C���̎��
	);

	if (image == nullptr)
	{
		SDL_Log("SOIL failed to load image %s: %s", fileName.c_str(), SOIL_last_result());
		return false;
	}
	
	int format = GL_RGB;
	if (channels == 4)		//�`�����l����4�Ȃ�RGBA
	{
		format = GL_RGBA;
	}
	
	//OpenGL�e�N�X�`���I�u�W�F�N�g�쐬(��2������id��ۑ�)
	glGenTextures(1, &mTextureID);
	//�e�N�X�`�����A�N�e�B�u��
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	
	//openGL�e�N�X�`���I�u�W�F�N�g�ɐ��̉摜�f�[�^���R�s�[
	glTexImage2D(
		GL_TEXTURE_2D,		//�e�N�X�`���^�[�Q�b�g
		0,		//Lod(Level of Detail)�ڍ׃��x��
		format, //OpenGL���g�p����J���[�t�H�[�}�b�g
		mWidth, 
		mHeight, 
		0,		//���E�l
		format,	//���̓f�[�^�̃J���[�t�H�[�}�b�g
		GL_UNSIGNED_BYTE, //���̓f�[�^�̃r�b�g�[�x(�����unsigned byte��8�r�b�g/�`�����l�����w�肵�Ă���)
		image);		//�摜�f�[�^�ւ̃|�C���^
	
	SOIL_free_image_data(image);		//�摜�f�[�^��OpenGL�ɃR�s�[������ASOIL�摜�f�[�^�̓������������ł���
	
	// Enable bilinear filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);		//glTexParameteri�Ńo�C�i���t�B���^(�F�̌v�Z������̃r�b�g�̏d���Ōv�Z������@)��L���ɂ���
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	return true;
}

void Texture::Unload()
{
	glDeleteTextures(1, &mTextureID);
}

void Texture::SetActive()
{
	//�V�F�[�_�[�Œ�`����uniform sampler2D uTexture�ɑ΂��āA����̃e�N�X�`�������̃o�C���h����c++�R�[�h�������Ă��Ȃ����Ƃ�����
	//�e�N�X�`��1�������o�C���h���Ă��Ȃ�����OpenGL�͂��̗B��̃e�N�X�`���T���v���[���A�N�e�B�u�e�N�X�`���ł���Ƃ������Ƃ������I�Ɏ@�m�ł��邩��
	glBindTexture(GL_TEXTURE_2D, mTextureID);
}
