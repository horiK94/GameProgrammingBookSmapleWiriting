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

//Texture::Load()はUIだけでなく、メッシュとして貼るテクスチャもロードされるので注意
bool Texture::Load(const std::string& fileName)
{
	int channels = 0;
	
	//テクスチャのロード
	unsigned char* image = SOIL_load_image(fileName.c_str(),
										   &mWidth, &mHeight, &channels, SOIL_LOAD_AUTO);
	
	if (image == nullptr)
	{
		SDL_Log("SOIL failed to load image %s: %s", fileName.c_str(), SOIL_last_result());
		return false;
	}
	
	//チャネル数を確認して、画像がRBGかRGBAか確認
	int format = GL_RGB;
	if (channels == 4)
	{
		format = GL_RGBA;
	}
	
	//OpenGL Texture Objectを作成
	glGenTextures(1, &mTextureID);
	//テクスチャをアクティブに
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	//生の画像データをコピー
	glTexImage2D(GL_TEXTURE_2D,	//テクスチャターゲット
		0,				//Lod(詳細レベル)
		format,			//OpenGLが使うべきカラーフォーマット
		mWidth,			//テクスチャの幅
		mHeight,		//テクスチャの高さ
		0,				//境界色
		format,			//Textureのカラーフォーマット
		GL_UNSIGNED_BYTE,		//入力データのビット深度(unsigned byteで8bit / chanel)
		image			//画像データのポインタ
	);
	
	SOIL_free_image_data(image);
	
	//テクスチャのフィルタリングを設定

	// Generate mipmaps for texture
	//glGenerateMipmap(GL_TEXTURE_2D);
	//ミップマッピングを有効化
	glGenerateMipmap(GL_TEXTURE_2D);
	// Enable linear filtering
	//トライリニアフィルタリングを有効にする
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);		//縮小用フィルタリング機能(トライリニアミップマッピング)
	//最近傍ミップマッピングを使用する場合は GL_LINEAR_MIPMAP_NEAREST を渡す
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);			//拡大用フィルタリング機能(バイリニアフィルタリング)
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//フィルタリングを設定したら、異方性フィルタリングを設定

	// Enable aniostropic filtering, if supported
	// OpenGL3.3では異方性フィルタリングはエクステンション(追加機能的な立ち位置)のため、グラフィックハードウェアが対応しているか確認する(大体のグラフィックハードウェアが対応している)
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
		//グラフィックハードウェアが異方性フィルタリングに対応しているなら

		//最大の異方性を示す値を取得
		GLfloat largest;
		//異方性の最大を表すパラメータ: GL_MAX_TEXTURE_MAX_ANISOTROPY. largestに異方性の最大値が代入されて帰ってくる
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &largest);
		//有効にする
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

//RGBAフォーマットであると決め打ちせず、formatで選択できるようにする
void Texture::CreateForRendering(int width, int height, unsigned int format)
{
	mWidth = width;
	mHeight = height;
	// Create the texture id
	//OpenGL Texture Objectの生成
	glGenTextures(1, &mTextureID);
	//テクスチャオブジェクトを有効に
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	// Set the image width/height with null initial data
	//初期データが無いため、nullptrを渡す(第2引数と後ろから2つ目の引数は無視される)
	//初期データはなし
	glTexImage2D(GL_TEXTURE_2D, 0, format, mWidth, mHeight, 0, GL_RGB,		//画像はAを取得できない(カメラから受け取れる画像データだから)
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
