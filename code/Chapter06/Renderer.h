// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <SDL/SDL.h>
#include "Math.h"
#include <map>

struct DirectionalLight
{
	//光源位置
	Vector3 mPosition;
	// Direction of light
	//Vector3 mDirection;
	// Diffuse color
	Vector3 mDiffuseColor;
	// Specular color
	Vector3 mSpecColor;

	//影響半径
	float mEffectRange;
};

class Renderer
{
public:
	Renderer(class Game* game);
	~Renderer();
	//初期化(Gameクラスが呼ぶ)
	bool Initialize(float screenWidth, float screenHeight);
	//終了
	void Shutdown();
	//テクスチャ, メッシュを解放
	void UnloadData();

	//Mesh, Spriteの描画(全MeshCompoent, SpriteComponentのDraw関数を呼んでいるだけ)
	void Draw();

	void AddSprite(class SpriteComponent* sprite);
	void RemoveSprite(class SpriteComponent* sprite);

	void Renderer::AddMeshComp(std::string shaderName, class MeshComponent* mesh);
	void RemoveMeshComp(class MeshComponent* mesh);

	class Texture* GetTexture(const std::string& fileName);		//画像データの取得
	class Mesh* GetMesh(const std::string& fileName);			//メッシュデータの取得

	void SetViewMatrix(const Matrix4& view) { mView = view; }

	void SetAmbientLight(const Vector3& ambient) { mAmbientLight = ambient; }
	DirectionalLight& GetDirectionalLight(int i) { return mDirLight[i]; }

	float GetScreenWidth() const { return mScreenWidth; }
	float GetScreenHeight() const { return mScreenHeight; }
private:
	bool LoadTextureShaders();
	bool LoadMeshShaders(std::string shaderName);		//initialize()時に呼ばれる
	void CreateSpriteVerts();
	void SetLightUniforms(class Shader* shader);

	// Map of textures loaded
	//ロードしたTexture
	std::unordered_map<std::string, class Texture*> mTextures;
	// Map of meshes loaded
	//ロードしたメッシュ
	std::unordered_map<std::string, class Mesh*> mMeshes;

	// All the sprite components drawn
	std::vector<class SpriteComponent*> mSprites;

	// All mesh components drawn
	std::map<std::string, std::vector<class MeshComponent*>> mMeshComps;

	// Game(未使用らしい)
	class Game* mGame;

	// Sprite shader
	class Shader* mSpriteShader;
	// Sprite vertex array
	class VertexArray* mSpriteVerts;

	// Mesh shader
	//class Shader* mMeshShader;
	std::map < std::string, class Shader*> mMeshShader;

	// View/projection for 3D shaders
	Matrix4 mView;
	Matrix4 mProjection;
	// Width/height of screen
	float mScreenWidth;
	float mScreenHeight;

	// Lighting data
	Vector3 mAmbientLight;
	DirectionalLight mDirLight[4];

	// Window
	SDL_Window* mWindow;
	// OpenGL context
	SDL_GLContext mContext;
};