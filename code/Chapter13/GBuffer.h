// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include <vector>

class GBuffer
{
public:
	// Different types of data stored in the G-buffer
	//Gバッファに格納されるデータの種類
	enum Type
	{
		EDiffuse = 0,		//albedo
		ENormal,			//法線
		EWorldPos,			//ワールド位置座標
		NUM_GBUFFER_TEXTURES		//GBUFFER TEXTUREの数
	};

	GBuffer();
	~GBuffer();

	//Gバッファの作成, 破棄
	// Create/destroy the G-buffer
	bool Create(int width, int height);
	void Destroy();
	
	// Get the texture for a specific type of data
	//指定するタイプのテクスチャを返す
	class Texture* GetTexture(Type type);
	// Get the framebuffer object ID
	//FBO(フレームバッファオブジェクト)のIDを返す
	unsigned int GetBufferID() const { return mBufferID; }
	// Setup all the G-buffer textures for sampling
	//すべてのGバッファを読み込み用に設定
	void SetTexturesActive();
private:
	// Textures associated with G-buffer
	//Gバッファとして割り当てられたTexture
	std::vector<class Texture*> mTextures;
	// Frame buffer object ID
	//FBO(フレームバッファオブジェクト)ID

	unsigned int mBufferID;
};
