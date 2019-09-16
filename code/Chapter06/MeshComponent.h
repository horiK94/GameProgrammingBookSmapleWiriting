// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "Component.h"
#include <cstddef>

class MeshComponent : public Component
{
public:
	MeshComponent(class Actor* owner);
	~MeshComponent();
	// Draw this mesh component
	//メッシュコンポーネントをどのShaderで描画するか決める
	virtual void Draw(class Shader* shader);
	// Set the mesh/texture index used by mesh component
	//メッシュコンポーネントが使う mesh, textureの指定
	virtual void SetMesh(class Mesh* mesh);
	Mesh* GetMesh() { return mMesh; }
	//textureのindexの指定
	void SetTextureIndex(size_t index) { mTextureIndex = index; }
protected:
	class Mesh* mMesh;
	size_t mTextureIndex;
};
