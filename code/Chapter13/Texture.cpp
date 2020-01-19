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

//Texture::Load()��UI�����łȂ��A���b�V���Ƃ��ē\��e�N�X�`�������[�h�����̂Œ���
bool Texture::Load(const std::string& fileName)
{
	int channels = 0;
	
	//�e�N�X�`���̃��[�h
	unsigned char* image = SOIL_load_image(fileName.c_str(),
										   &mWidth, &mHeight, &channels, SOIL_LOAD_AUTO);
	
	if (image == nullptr)
	{
		SDL_Log("SOIL failed to load image %s: %s", fileName.c_str(), SOIL_last_result());
		return false;
	}
	
	//�`���l�������m�F���āA�摜��RBG��RGBA���m�F
	int format = GL_RGB;
	if (channels == 4)
	{
		format = GL_RGBA;
	}
	
	//OpenGL Texture Object���쐬
	glGenTextures(1, &mTextureID);
	//�e�N�X�`�����A�N�e�B�u��
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	//���̉摜�f�[�^���R�s�[
	glTexImage2D(GL_TEXTURE_2D,	//�e�N�X�`���^�[�Q�b�g
		0,				//Lod(�ڍ׃��x��)
		format,			//OpenGL���g���ׂ��J���[�t�H�[�}�b�g
		mWidth,			//�e�N�X�`���̕�
		mHeight,		//�e�N�X�`���̍���
		0,				//���E�F
		format,			//Texture�̃J���[�t�H�[�}�b�g
		GL_UNSIGNED_BYTE,		//���̓f�[�^�̃r�b�g�[�x(unsigned byte��8bit / chanel)
		image			//�摜�f�[�^�̃|�C���^
	);
	
	SOIL_free_image_data(image);
	
	//�e�N�X�`���̃t�B���^�����O��ݒ�

	// Generate mipmaps for texture
	//glGenerateMipmap(GL_TEXTURE_2D);
	//�~�b�v�}�b�s���O��L����
	glGenerateMipmap(GL_TEXTURE_2D);
	// Enable linear filtering
	//�g���C���j�A�t�B���^�����O��L���ɂ���
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);		//�k���p�t�B���^�����O�@�\(�g���C���j�A�~�b�v�}�b�s���O)
	//�ŋߖT�~�b�v�}�b�s���O���g�p����ꍇ�� GL_LINEAR_MIPMAP_NEAREST ��n��
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);			//�g��p�t�B���^�����O�@�\(�o�C���j�A�t�B���^�����O)
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//�t�B���^�����O��ݒ肵����A�ٕ����t�B���^�����O��ݒ�

	// Enable aniostropic filtering, if supported
	// OpenGL3.3�ł͈ٕ����t�B���^�����O�̓G�N�X�e���V����(�ǉ��@�\�I�ȗ����ʒu)�̂��߁A�O���t�B�b�N�n�[�h�E�F�A���Ή����Ă��邩�m�F����(��̂̃O���t�B�b�N�n�[�h�E�F�A���Ή����Ă���)
	//if (GLEW_EXT_texture_filter_anisotropic)
	//{
	//	// Get the maximum anisotropy value
	//	GLfloat largest;
	//	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest);
	//	// Enable it
	//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, largest);
	//}
	if (GLEW_EXT_texture_filter_anisotropic)
	{
		//�O���t�B�b�N�n�[�h�E�F�A���ٕ����t�B���^�����O�ɑΉ����Ă���Ȃ�

		//�ő�ٕ̈����������l���擾
		GLfloat largest;
		//�ٕ����̍ő��\���p�����[�^: GL_MAX_TEXTURE_MAX_ANISOTROPY. largest�Ɉٕ����̍ő�l���������ċA���Ă���
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &largest);
		//�L���ɂ���
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, largest);
	}
	
	return true;
}

void Texture::Unload()
{
	glDeleteTextures(1, &mTextureID);
}

void Texture::CreateFromSurface(SDL_Surface* surface)
{
	mWidth = surface->w;
	mHeight = surface->h;
	
	// Generate a GL texture
	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_BGRA,
				 GL_UNSIGNED_BYTE, surface->pixels);
	
	// Use linear filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

//RGBA�t�H�[�}�b�g�ł���ƌ��ߑł������Aformat�őI���ł���悤�ɂ���
void Texture::CreateForRendering(int width, int height, unsigned int format)
{
	mWidth = width;
	mHeight = height;
	// Create the texture id
	//OpenGL Texture Object�̐���
	glGenTextures(1, &mTextureID);
	//�e�N�X�`���I�u�W�F�N�g��L����
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	// Set the image width/height with null initial data
	//�����f�[�^���������߁Anullptr��n��(��2�����ƌ�납��2�ڂ̈����͖��������)
	//�����f�[�^�͂Ȃ�
	glTexImage2D(GL_TEXTURE_2D, 0, format, mWidth, mHeight, 0, GL_RGB,		//�摜��A���擾�ł��Ȃ�(�J��������󂯎���摜�f�[�^������)
		GL_FLOAT, nullptr);

	// For a texture we'll render to, just use nearest neighbor
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void Texture::SetActive(int index)
{
	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D, mTextureID);
}
