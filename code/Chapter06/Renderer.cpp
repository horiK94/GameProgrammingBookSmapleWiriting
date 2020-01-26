// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Renderer.h"
#include "Texture.h"
#include "Mesh.h"
#include <algorithm>
#include "Shader.h"
#include "VertexArray.h"
#include "SpriteComponent.h"
#include "MeshComponent.h"
#include <GL/glew.h>
#include <map>
#include <iostream>
#include <sstream>

Renderer::Renderer(Game* game)
	:mGame(game)
	, mSpriteShader(nullptr)
	, mMeshShader(std::map < std::string, class Shader* >())
{
}

Renderer::~Renderer()
{
}

bool Renderer::Initialize(float screenWidth, float screenHeight)
{
	mScreenWidth = screenWidth;
	mScreenHeight = screenHeight;

	// Set OpenGL attributes
	// Use the core OpenGL profile
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	// Specify version 3.3
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	// Request a color buffer with 8-bits per RGBA channel
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	//SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	//デプスバッファのリクエストにする
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	// Enable double buffering
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	// Force OpenGL to use hardware acceleration
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

	mWindow = SDL_CreateWindow("Game Programming in C++ (Chapter 6)", 100, 100,
		static_cast<int>(mScreenWidth), static_cast<int>(mScreenHeight), SDL_WINDOW_OPENGL);
	if (!mWindow)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}

	// Create an OpenGL context
	mContext = SDL_GL_CreateContext(mWindow);

	// Initialize GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		SDL_Log("Failed to initialize GLEW.");
		return false;
	}

	// On some platforms, GLEW will emit a benign error code,
	// so clear it
	glGetError();

	// Make sure we can create/compile shaders
	if (!LoadTextureShaders())
	{
		SDL_Log("Failed to load shaders.");
		return false;
	}
	//if (!LoadMeshShaders())
	//{
	//	SDL_Log("Failed to load shaders.");
	//	return false;
	//}

	// Create quad for drawing sprites
	CreateSpriteVerts();

	return true;
}

void Renderer::Shutdown()
{
	delete mSpriteVerts;
	mSpriteShader->Unload();
	delete mSpriteShader;
	for (auto m : mMeshShader)
	{
		m.second->Unload();
	}
	mMeshShader.clear();
	SDL_GL_DeleteContext(mContext);
	SDL_DestroyWindow(mWindow);
}

void Renderer::UnloadData()
{
	// Destroy textures
	for (auto i : mTextures)
	{
		i.second->Unload();
		delete i.second;
	}
	mTextures.clear();

	// Destroy meshes
	for (auto i : mMeshes)
	{
		i.second->Unload();
		delete i.second;
	}
	mMeshes.clear();
}

void Renderer::Draw()
{
	//Meshの描画
	// Set the clear color to light grey
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Clear the color buffer
	//デプスバッファとカラーバッファをクリアする
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw mesh components
	// Enable depth buffering/disable alpha blend
	//デプスバッファ法を有効にする(Drawするたびに呼ぶ)
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_DEPTH_TEST);
	//アルファブレンディングを無効化
	glDisable(GL_BLEND);

	for (auto m : mMeshShader)
	{
		std::string shaderName = m.first;
		Shader* shader = m.second;
		// Set the mesh shader active
		//meshShaderをアクティブ化
		shader->SetActive();
		// Update view-projection matrix
		//meshShaderに対し、カメラの位置に応じたビュー射影行列を設定
		shader->SetMatrixUniform("uViewProj", mView * mProjection);
		// Update lighting uniforms
		SetLightUniforms(shader);

		std::vector<MeshComponent*> meshComponents = mMeshComps[shaderName];
		//全MeshComponentに対し、描画を行う
		for (auto mc : meshComponents)
		{
			mc->Draw(shader);
		}
	}

	//Spriteの描画
	// Draw all sprite components
	// Disable depth buffering
	glDisable(GL_DEPTH_TEST);
	// Enable alpha blending on the color buffer
	glEnable(GL_BLEND);
	glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

	// Set shader/vao as active
	mSpriteShader->SetActive();
	mSpriteVerts->SetActive();
	for (auto sprite : mSprites)
	{
		sprite->Draw(mSpriteShader);
	}

	// Swap the buffers
	SDL_GL_SwapWindow(mWindow);
}

void Renderer::AddSprite(SpriteComponent* sprite)
{
	// Find the insertion point in the sorted vector
	// (The first element with a higher draw order than me)
	int myDrawOrder = sprite->GetDrawOrder();
	auto iter = mSprites.begin();
	for (;
		iter != mSprites.end();
		++iter)
	{
		if (myDrawOrder < (*iter)->GetDrawOrder())
		{
			break;
		}
	}

	// Inserts element before position of iterator
	mSprites.insert(iter, sprite);
}

void Renderer::RemoveSprite(SpriteComponent* sprite)
{
	auto iter = std::find(mSprites.begin(), mSprites.end(), sprite);
	mSprites.erase(iter);
}

void Renderer::AddMeshComp(std::string shaderName, MeshComponent* mesh)
{
	mMeshComps[shaderName].push_back(mesh);
	//本当は初期化時にやりたいが、RendereコンポーネントはGameの初期化時の最初におこわなれるためMeshComponent時に行う
	if (!LoadMeshShaders(shaderName))
	{
		SDL_Log("Failed to load shaders.");
	}
}

void Renderer::RemoveMeshComp(MeshComponent* mesh)
{
	//auto iter = std::find(mMeshComps.begin(), mMeshComps.end(), mesh);
	//mMeshComps.erase(iter);
	std::vector<MeshComponent*> meshComps = mMeshComps[mesh->GetMesh()->GetShaderName()];
	auto iter = std::find(meshComps.begin(), meshComps.end(), mesh);
	meshComps.erase(iter);
}

Texture* Renderer::GetTexture(const std::string& fileName)
{
	Texture* tex = nullptr;
	auto iter = mTextures.find(fileName);		//key..string, value..Texture*
	if (iter != mTextures.end())		//RendereのmTexturesの中にfileNameのものがあるとき
	{
		tex = iter->second;		//texにiterのvalue(Texture)を代入
	}
	else
	{
		tex = new Texture();		//Textureインスタンス生成
		if (tex->Load(fileName))		//fileNameのものをロード
		{
			mTextures.emplace(fileName, tex);		//mTexturesに追加(順番は保持されない)
		}
		else
		{				//なかったら諦める
			delete tex;
			tex = nullptr;
		}
	}
	return tex;
}

Mesh* Renderer::GetMesh(const std::string& fileName)
{
	Mesh* m = nullptr;
	auto iter = mMeshes.find(fileName);
	if (iter != mMeshes.end())
	{
		m = iter->second;
	}
	else
	{
		m = new Mesh();
		if (m->Load(fileName, this))
		{
			mMeshes.emplace(fileName, m);
		}
		else
		{
			delete m;
			m = nullptr;
		}
	}
	return m;
}

bool Renderer::LoadTextureShaders()
{
	// Create sprite shader
	mSpriteShader = new Shader();
	if (!mSpriteShader->Load("Shaders/Sprite.vert", "Shaders/Sprite.frag"))
	{
		return false;
	}

	mSpriteShader->SetActive();
	// Set the view-projection matrix
	Matrix4 viewProj = Matrix4::CreateSimpleViewProj(mScreenWidth, mScreenHeight);
	mSpriteShader->SetMatrixUniform("uViewProj", viewProj);
}

bool Renderer::LoadMeshShaders(std::string shaderName)
{
	if (mMeshShader.find(shaderName) != end(mMeshShader))
	{
		return true;
	}

	// Create basic mesh shader
	Shader* shader = new Shader();
	if (!shader->Load("Shaders/" + shaderName + ".vert", "Shaders/" + shaderName + ".frag"))
	{
		return false;
	}
	//mMeshShader = new Shader();
	//if (!mMeshShader->Load("Shaders/BasicMesh.vert", "Shaders/BasicMesh.frag"))
	//{
	//	return false;
	//}

	shader->SetActive();
	//ビューと射影行列を設定
	//ビュー行列の作成
	mView = Matrix4::CreateLookAt(
		Vector3::Zero,		//カメラの位置
		Vector3::UnitX,		//ターゲットの位置
		Vector3::UnitZ);	//カメラの上向き

	mProjection = Matrix4::CreatePerspectiveFOV(
		Math::ToRadians(70.0f),		//水平視野
		mScreenWidth,
		mScreenHeight,
		25.0f,
		10000.f
	);
	shader->SetMatrixUniform("uViewProj", mView * mProjection);

	mMeshShader[shaderName] = shader;

	//mMeshShader->SetActive();
	//// Set the view-projection matrix
	//mView = Matrix4::CreateLookAt(Vector3::Zero, Vector3::UnitX, Vector3::UnitZ);
	//mProjection = Matrix4::CreatePerspectiveFOV(Math::ToRadians(70.0f),
	//	mScreenWidth, mScreenHeight, 25.0f, 10000.0f);
	//mMeshShader->SetMatrixUniform("uViewProj", mView * mProjection);
	return true;
}

void Renderer::CreateSpriteVerts()
{
	float vertices[] = {
		-0.5f, 0.5f, 0.f, 0.f, 0.f, 0.0f, 0.f, 0.f, // top left
		0.5f, 0.5f, 0.f, 0.f, 0.f, 0.0f, 1.f, 0.f, // top right
		0.5f,-0.5f, 0.f, 0.f, 0.f, 0.0f, 1.f, 1.f, // bottom right
		-0.5f,-0.5f, 0.f, 0.f, 0.f, 0.0f, 0.f, 1.f  // bottom left
	};

	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	mSpriteVerts = new VertexArray(vertices, 4, indices, 6);
}

void Renderer::SetLightUniforms(Shader* shader)
{
	// Camera position is from inverted view
	//Matrix4 invView = mView;
	//invView.Invert();
	//shader->SetVectorUniform("uCameraPos", invView.GetTranslation());
	//// Ambient light
	//shader->SetVectorUniform("uAmbientLight", mAmbientLight);
	//// Directional light
	//shader->SetVectorUniform("uDirLight.mDirection",
	//	mDirLight.mDirection);
	//shader->SetVectorUniform("uDirLight.mDiffuseColor",
	//	mDirLight.mDiffuseColor);
	//shader->SetVectorUniform("uDirLight.mSpecColor",
	//	mDirLight.mSpecColor);
	Matrix4 inView = mView;		//ビュー行列(カメラを中心とした座標)
	inView.Invert();		//逆行列
	shader->SetVectorUniform("uCameraPos", inView.GetTranslation());
	shader->SetVectorUniform("uAmbientLight", mAmbientLight);
	for (int i = 0; i < 4; i++)
	{
		//uDirLight構造体に「.」で参照できる
		std::ostringstream ossPos;
		ossPos << "uDirLight[" << i << "].mPosition";
		shader->SetVectorUniform(ossPos.str().c_str(), mDirLight[i].mPosition);
		//shader->SetVectorUniform("uDirLight.mDirection", mDirLight.mDirection);

		std::ostringstream ossDiffuseColor;
		ossDiffuseColor << "uDirLight[" << i << "].mDiffuseColor";
		shader->SetVectorUniform(ossDiffuseColor.str().c_str(), mDirLight[i].mDiffuseColor);

		std::ostringstream ossSpecColor;
		ossSpecColor << "uDirLight[" << i << "].mSpecColor";
		shader->SetVectorUniform(ossSpecColor.str().c_str(), mDirLight[i].mSpecColor);

		std::ostringstream ossEffect;
		ossEffect << "uDirLight[" << i << "].mEffectRange";
		shader->SetFloatUniform(ossEffect.str().c_str(), mDirLight[i].mEffectRange);
	}
}
