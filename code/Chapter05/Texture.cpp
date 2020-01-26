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
		&mWidth,		//幅が記録される
		&mHeight,
		&channels,
		SOIL_LOAD_AUTO		//画像ファイルの種類
	);

	if (image == nullptr)
	{
		SDL_Log("SOIL failed to load image %s: %s", fileName.c_str(), SOIL_last_result());
		return false;
	}
	
	int format = GL_RGB;
	if (channels == 4)		//チャンネルが4つならRGBA
	{
		format = GL_RGBA;
	}
	
	//OpenGLテクスチャオブジェクト作成(第2引数でidを保存)
	glGenTextures(1, &mTextureID);
	//テクスチャをアクティブに
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	
	//openGLテクスチャオブジェクトに生の画像データをコピー
	glTexImage2D(
		GL_TEXTURE_2D,		//テクスチャターゲット
		0,		//Lod(Level of Detail)詳細レベル
		format, //OpenGLが使用するカラーフォーマット
		mWidth, 
		mHeight, 
		0,		//境界値
		format,	//入力データのカラーフォーマット
		GL_UNSIGNED_BYTE, //入力データのビット深度(今回はunsigned byteで8ビット/チャンネルを指定している)
		image);		//画像データへのポインタ
	
	SOIL_free_image_data(image);		//画像データをOpenGLにコピーしたら、SOIL画像データはメモリから解放できる
	
	// Enable bilinear filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);		//glTexParameteriでバイナリフィルタ(色の計算を周りのビットの重さで計算する方法)を有効にする
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	return true;
}

void Texture::Unload()
{
	glDeleteTextures(1, &mTextureID);
}

void Texture::SetActive()
{
	//シェーダーで定義したuniform sampler2D uTextureに対して、今回のテクスチャを何故バインドするc++コードを書いていないかというと
	//テクスチャ1枚しかバインドしていないためOpenGLはこの唯一のテクスチャサンプラーがアクティブテクスチャであるということを自動的に察知できるから
	glBindTexture(GL_TEXTURE_2D, mTextureID);
}
