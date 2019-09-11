// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include <vector>
#include <string>

class Mesh		//メッシュデータ
{
public:
	Mesh();
	~Mesh();
	// Load/unload mesh
	//ロードしたTextureを持つためrendereのポインタを受け取る
	bool Load(const std::string& fileName, class Renderer* renderer);
	void Unload();
	// Get the vertex array associated with this mesh
	//メッシュに割り当てられた頂点配列(頂点バッファ, インデックスバッファ)を取得
	class VertexArray* GetVertexArray() { return mVertexArray; }
	// Get a texture from specified index
	//テクスチャの取得
	class Texture* GetTexture(size_t index);
	// Get name of shader
	//シェーダー名の取得
	const std::string& GetShaderName() const { return mShaderName; }
	// Get object space bounding sphere radius
	float GetRadius() const { return mRadius; }
	// Get specular power of mesh
	float GetSpecPower() const { return mSpecPower; }
private:
	// Textures associated with this mesh
	//メッシュのテクスチャ群
	std::vector<class Texture*> mTextures;
	// Vertex array associated with this mesh
	//頂点配列のポインタ
	class VertexArray* mVertexArray;
	// Name of shader specified by mesh
	//シェーダー名
	std::string mShaderName;
	// Stores object space bounding sphere radius
	//半径
	float mRadius;
	// Specular power of surface
	//鏡面反射指数
	float mSpecPower;
};