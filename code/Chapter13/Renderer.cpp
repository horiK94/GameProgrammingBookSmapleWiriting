// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE.txt for full details.
// ----------------------------------------------------------------

#include "Renderer.h"
#include "Texture.h"
#include "Mesh.h"
#include <algorithm>
#include "Shader.h"
#include "VertexArray.h"
#include "SpriteComponent.h"
#include "MeshComponent.h"
#include "UIScreen.h"
#include "Game.h"
#include <GL/glew.h>
#include "SkeletalMeshComponent.h"
#include "GBuffer.h"
#include "PointLightComponent.h"

Renderer::Renderer(Game* game)
	:mGame(game)
	,mSpriteShader(nullptr)
	,mMeshShader(nullptr)
	,mSkinnedShader(nullptr)
	,mMirrorBuffer(0)
	,mMirrorTexture(nullptr)
	,mGBuffer(nullptr)
	,mGGlobalShader(nullptr)
	,mGPointLightShader(nullptr)
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
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	// Enable double buffering
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	// Force OpenGL to use hardware acceleration
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

	mWindow = SDL_CreateWindow("Game Programming in C++ (Chapter 13)", 100, 100,
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
	if (!LoadShaders())
	{
		SDL_Log("Failed to load shaders.");
		return false;
	}

	// Create quad for drawing sprites
	CreateSpriteVerts();

	// Create render target for mirror
	//ミラーのフレームバッファオブジェクトの作成
	if (!CreateMirrorTarget())
	{
		SDL_Log("Failed to create render target for mirror.");
		return false;
	}
	
	// Create G-buffer
	//GBufferの作成
	mGBuffer = new GBuffer();
	int width = static_cast<int>(mScreenWidth);
	int height = static_cast<int>(mScreenHeight);
	if (!mGBuffer->Create(width, height))
	{
		SDL_Log("Failed to create G-buffer.");
		return false;
	}

	// Load point light mesh
	//球のメッシュのロード
	mPointLightMesh = GetMesh("Assets/PointLight.gpmesh");

	return true;
}

void Renderer::Shutdown()
{
	// Get rid of any render target textures, if they exist
	if (mMirrorTexture != nullptr)
	{
		glDeleteFramebuffers(1, &mMirrorBuffer);
		mMirrorTexture->Unload();
		delete mMirrorTexture;
	}
	// Get rid of G-buffer
	if (mGBuffer != nullptr)
	{
		mGBuffer->Destroy();
		delete mGBuffer;
	}
	// Delete point lights
	while (!mPointLights.empty())
	{
		delete mPointLights.back();
	}
	delete mSpriteVerts;
	mSpriteShader->Unload();
	delete mSpriteShader;
	mMeshShader->Unload();
	delete mMeshShader;
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
	// Draw to the mirror texture first
	//ミラーのテクスチャへの描画
	Draw3DScene(mMirrorBuffer, mMirrorView, mProjection, 0.25f);
	// Draw the 3D scene to the G-buffer
	//3DシーンをGbafferへ描画(mMeshShaderもmSkinnedShaderもフラグメントシェーダーでlightingの設定をしないため、
	//ライティングに関するuniformの設定は不要) => lit=falseで設定するとライティングの設定は行われなくなる
	Draw3DScene(mGBuffer->GetBufferID()/*0*/, mView, mProjection, 1.0f, false);
	//// Set the frame buffer back to zero (screen's frame buffer)
	//フレームバッファをデフォルトのフレームバッファ(id: 0)に戻すよう設定
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//// Draw from the GBuffer
	//Gバッファに保存された情報をもとにデフォルトのフレームバッファに描画
	DrawFromGBuffer();

	//SpriteとUIはデフォルトフレームバッファに書きこむ
	
	// Draw all sprite components
	// Disable depth buffering
	//デプスバッファの無効化
	glDisable(GL_DEPTH_TEST);
	// Enable alpha blending on the color buffer
	//アルファブレンディングの有効
	glEnable(GL_BLEND);
	//TODO: 下2行調べる
	glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

	// Set shader/vao as active
	//Spriteの描画
	mSpriteShader->SetActive();
	mSpriteVerts->SetActive();
	for (auto sprite : mSprites)
	{
		if (sprite->GetVisible())
		{
			sprite->Draw(mSpriteShader);
		}
	}
	
	// Draw any UI screens
	//UIの描画(escapeとか押したときのUIは押したときにGetUIStack()から受け取れるポインタに追加されている)
	//ミラーのTextureもUIとして描画(だから前に描画される)
	for (auto ui : mGame->GetUIStack())
	{
		ui->Draw(mSpriteShader);
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

void Renderer::AddMeshComp(MeshComponent* mesh)
{
	if (mesh->GetIsSkeletal())
	{
		SkeletalMeshComponent* sk = static_cast<SkeletalMeshComponent*>(mesh);
		mSkeletalMeshes.emplace_back(sk);
	}
	else
	{
		mMeshComps.emplace_back(mesh);
	}
}

void Renderer::RemoveMeshComp(MeshComponent* mesh)
{
	if (mesh->GetIsSkeletal())
	{
		SkeletalMeshComponent* sk = static_cast<SkeletalMeshComponent*>(mesh);
		auto iter = std::find(mSkeletalMeshes.begin(), mSkeletalMeshes.end(), sk);
		mSkeletalMeshes.erase(iter);
	}
	else
	{
		auto iter = std::find(mMeshComps.begin(), mMeshComps.end(), mesh);
		mMeshComps.erase(iter);
	}
}

void Renderer::AddPointLight(PointLightComponent* light)
{
	mPointLights.emplace_back(light);
}

void Renderer::RemovePointLight(PointLightComponent* light)
{
	auto iter = std::find(mPointLights.begin(), mPointLights.end(), light);
	mPointLights.erase(iter);
}

Texture* Renderer::GetTexture(const std::string& fileName)
{
	Texture* tex = nullptr;
	auto iter = mTextures.find(fileName);
	if (iter != mTextures.end())
	{
		tex = iter->second;
	}
	else
	{
		tex = new Texture();
		if (tex->Load(fileName))
		{
			mTextures.emplace(fileName, tex);
		}
		else
		{
			delete tex;
			tex = nullptr;
		}
	}
	return tex;
}

Mesh* Renderer::GetMesh(const std::string & fileName)
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

//Shadernに値を送っている
//frameBuffer: フレームバッファID
//view: ビュー行列
// proj: 射影行列
//viewPortScale: ビューポートのスケール(これにより、OpenGLはフレームバッファターゲットの実際のサイズを知る)
//バックミラーのために、ビューポートをスケーリングするパラメータ
//lit: falseならライティングの定数を設定しないよう指示
void Renderer::Draw3DScene(unsigned int framebuffer, const Matrix4& view, const Matrix4& proj, 
	float viewPortScale, bool lit)
{
	// Set the current frame buffer
	//これから書きこむフレームバッファに指定
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	// Set viewport size based on scale
	//スケールに基づいてビューポートサイズを設定
	glViewport(0, 0,
		static_cast<int>(mScreenWidth * viewPortScale),
		static_cast<int>(mScreenHeight * viewPortScale)
	);

	// Clear color buffer/depth buffer
	//グレーでカラーを初期化
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	//深度バッファへの書き込みを許可する(DrawFrameGBuffer()で点光源を描画する際に、
	//深度テストの書き込みを禁止しているので、ここで許可にする必要がある)
	//glDepthMask: 深度バッファの書き込みを有効/無効 にする
	glDepthMask(GL_TRUE);
	//カラーバッファ、デプスバッファのクリア
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw mesh components
	//メッシュコンポーネントの描画
	// Enable depth buffering/disable alpha blend
	//デプスバッファの有効
	glEnable(GL_DEPTH_TEST);
	//アルファブレンディングの無効
	glDisable(GL_BLEND);
	// Set the mesh shader active
	mMeshShader->SetActive();
	// Update view-projection matrix
	mMeshShader->SetMatrixUniform("uViewProj", view * proj);
	// Update lighting uniforms
	//mMeshShaderはGBufferWrite.fragだからlightingに関するuniformを設定する必要がない
	if (lit)
	{
		SetLightUniforms(mMeshShader, view);
	}
	for (auto mc : mMeshComps)
	{
		if (mc->GetVisible())
		{
			mc->Draw(mMeshShader);
		}
	}

	//スケルタルメッシュコンポーネントの描画
	// Draw any skinned meshes now
	mSkinnedShader->SetActive();
	// Update view-projection matrix
	mSkinnedShader->SetMatrixUniform("uViewProj", view * proj);
	// Update lighting uniforms
	//mSkinnedShaderはGBufferWrite.fragだからlightingに関するuniformを設定する必要がない
	if (lit)
	{
		SetLightUniforms(mSkinnedShader, view);
	}
	for (auto sk : mSkeletalMeshes)
	{
		if (sk->GetVisible())
		{
			sk->Draw(mSkinnedShader);
		}
	}
}

///バックミラーのフレームバッファオブジェクトの作成
bool Renderer::CreateMirrorTarget()
{
	//サイズを1/4にする
	int width = static_cast<int>(mScreenWidth) / 4;
	int height = static_cast<int>(mScreenHeight) / 4;

	// Generate a frame buffer for the mirror texture
	//フレームバッファオブジェクトの作成(IDの取得)
	glGenFramebuffers(1, &mMirrorBuffer);
	//アクティブにする
	glBindFramebuffer(GL_FRAMEBUFFER, mMirrorBuffer);

	// Create the texture we'll use for rendering
	//レンダリングに使うテクスチャの初期化
	mMirrorTexture = new Texture();
	//レンダリング用テクスチャの作成(大きさはwidth*height. 視点から見た色を出力するためにRGBを指定)
	mMirrorTexture->CreateForRendering(width, height, GL_RGB);

	// Add a depth buffer to this target
	//デプス(深度)バッファの追加
	GLuint depthBuffer;
	//デプスバッファの作成(IDの取得)(正確にはレンダーバッファ作成: 保持できる値は各ピクセルの特定の値(color, depthなど))
	glGenRenderbuffers(1, &depthBuffer);
	//レンダーバッファをアクティブにする
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);

	//作成したレンダーバッファにGL_DEPTH_COMPONENT: デプスバッファ ストレージとして使用することを宣言
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	// フレームバッファにレンダーバッファを、デプスバッファとしてアタッチ
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

	// Attach mirror texture as the output target for the frame buffer
	//バックミラー用Textureをバックミラー用フレームバッファオブジェクトに割り当てる
	//GL_COLOR_ATTACHMENT0: フラグメントシェーダーの最初の色出力に対応しますよという意味
	//フラグメントシェーダーは今まで1つのみの出力だったが複数のテクスチャに出力できる(複数の出力ができる)
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mMirrorTexture->GetTextureID(), 0);

	// Set the list of buffers to draw to for this frame buffer
	//今回のフレームバッファオブジェクトではGL_COLOR_ATTACHMENT0スロットのTexture(つまり、mMirrorTexture)に描画することを設定
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);

	// Make sure everything worked
	//glCheckFramebufferStatusで状態チェック
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		// If it didn't work, delete the framebuffer,
		// unload/delete the texture and return false
		//問題があるならフレームバッファを消し、テクスチャを解放、削除してfalseを返す
		glDeleteFramebuffers(1, &mMirrorBuffer);
		mMirrorTexture->Unload();
		delete mMirrorTexture;
		mMirrorTexture = nullptr;
		return false;
	}
	return true;
}

void Renderer::DrawFromGBuffer()
{
	// Disable depth testing for the global lighting pass
	//グローバルライティングパスでは深度テストは禁止(すでにしたものがGバッファに保存されているため)
	glDisable(GL_DEPTH_TEST);
	// Activate global G-buffer shader
	//GバッファシェーダーをActiveに(頂点配列の情報はactiveにしていない状態)
	mGGlobalShader->SetActive();
	// Activate sprite verts quad
	//矩形の頂点配列をアクティブにした(Shaderではなく、頂点配列がActiveに)
	mSpriteVerts->SetActive();
	// Set the G-buffer textures to sample
	//サンプリングするGバッファのテクスチャをアクティブに(これによりuniformが対応する)
	mGBuffer->SetTexturesActive();
	// Set the lighting uniforms
	//ライトのuniform設定を行う
	SetLightUniforms(mGGlobalShader, mView);
	// Draw the triangles
	//矩形のための三角形を描画
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

	//点光源の反映

	// Copy depth buffer from G-buffer to default frame buffer
	//GL_READ_FRAMEBUFFER: 読み専用フレームバッファとしてmGBuffer->GetBufferID()のものを設定している
	glBindFramebuffer(GL_READ_FRAMEBUFFER, mGBuffer->GetBufferID());
	int width = static_cast<int>(mScreenWidth);
	int height = static_cast<int>(mScreenHeight);
	//gglBlitFramebufferの場合、読み取りおよび描画フレームバッファーは、
	//それぞれGL_READ_FRAMEBUFFERおよびGL_DRAW_FRAMEBUFFERターゲットにバインドされています。
	//つまり、上でmGBuffer->GetBufferID()が読み込み用フレームバッファとして設定されており、
	//DrawFromGBuffer()の前に glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//としているため、デフォルトフーレムバッファを書き込み用として設定している
	glBlitFramebuffer(0, 0, width, height,		//ソースとなるピクセルの個数
		0, 0, width, height,		//目的側のピクセルの数
		GL_DEPTH_BUFFER_BIT, //	コピーするバッファを示すフラグのビット単位のOR。
		//許可されるフラグはGL_COLOR_BUFFER_BIT、GL_DEPTH_BUFFER_BIT、およびGL_STENCIL_BUFFER_BIT
		//今回はdepthのみコピーしたいのでGL_DEPTH_BUFFER_BIT
		GL_NEAREST);	//画像が引き伸ばされた場合に適用される補間を指定します。 
		//GL_NEARESTまたはGL_LINEARでなければなりません。

	// Enable depth test, but disable writes to depth buffer
	//depthテストを有効にする
	glEnable(GL_DEPTH_TEST);
	//深度バッファへの書き込みはしない(深度テスト中は深度に関する情報を持っているが、最後にバッファに書きこむことはしない)
	//これにより、点光源の球形メッシュが既存のバッファの値に影響を与えない
	glDepthMask(GL_FALSE);

	// Set the point light shader and mesh as active
	//mGPointLightShaderのシェーダーをアクティブに
	mGPointLightShader->SetActive();
	//球体メッシュの頂点配列をアクティブに
	mPointLightMesh->GetVertexArray()->SetActive();
	// Set the view-projeciton matrix
	//ビュー射影行列の設定
	//uWorldTransform, uPointLightは各mPointLightsのPointLightComponentコンポーネントのDraw()で, uScreenDimensionsはLoadShaders()で設定
	mGPointLightShader->SetMatrixUniform("uViewProj",
		mView * mProjection);
	// Set the G-buffer textures for sampling
	//サンプリングするGバッファのテクスチャをアクティブに(これによりuniformがmGPointLightShaderのuniformに対応する)
	mGBuffer->SetTexturesActive();

	// The point light color should add to existing color
	//混合処理は、画面のピクセルの色に新しい色を組み合わせる処理です
	//混合処理を使用しない場合は、新しい色が現在のピクセルの色を上書きします
	//ブレンディングを有効にするには以下の関数を呼ばなければなりません。
	//glEnable(GL_BLEND);
	glEnable(GL_BLEND);
	//glBlendFunc では、それぞれの画像の RGBA にかける係数を設定
	//GL_ZERO RGBA 全てそのまま => (0, 0, 0, 0)(白に近くなっていく) ということ
	//単純に足していく場合はこれで
	glBlendFunc(GL_ONE, GL_ONE);

	// Draw the point lights
	//点光源の描画(既存のid: 0のデフォルトフレームバッファに対して、足していく形で処理をしていく)
	for (PointLightComponent* p : mPointLights)
	{
		p->Draw(mGPointLightShader, mPointLightMesh);
	}
}

bool Renderer::LoadShaders()
{
	// Create sprite shader
	mSpriteShader = new Shader();
	if (!mSpriteShader->Load("Shaders/Sprite.vert", "Shaders/Sprite.frag"))
	{
		return false;
	}

	mSpriteShader->SetActive();
	// Set the view-projection matrix
	Matrix4 spriteViewProj = Matrix4::CreateSimpleViewProj(mScreenWidth, mScreenHeight);
	mSpriteShader->SetMatrixUniform("uViewProj", spriteViewProj);

	// Create basic mesh shader
	mMeshShader = new Shader();
	//フラグメントシェーダーとしてGBufferWrite.fragを使うようにする
	if (!mMeshShader->Load("Shaders/Phong.vert", "Shaders/GBufferWrite.frag"))
	{
		return false;
	}

	mMeshShader->SetActive();
	// Set the view-projection matrix
	mView = Matrix4::CreateLookAt(Vector3::Zero, Vector3::UnitX, Vector3::UnitZ);
	mProjection = Matrix4::CreatePerspectiveFOV(Math::ToRadians(70.0f),
		mScreenWidth, mScreenHeight, 10.0f, 10000.0f);
	mMeshShader->SetMatrixUniform("uViewProj", mView * mProjection);

	// Create skinned shader
	mSkinnedShader = new Shader();
	//フラグメントシェーダーとしてGBufferWrite.fragを使うようにする
	if (!mSkinnedShader->Load("Shaders/Skinned.vert", "Shaders/GBufferWrite.frag"))
	{
		return false;
	}

	mSkinnedShader->SetActive();
	mSkinnedShader->SetMatrixUniform("uViewProj", mView * mProjection);
	
	// Create shader for drawing from GBuffer (global lighting)
	// GBUfferグローバルライティングシェーダーのロード
	mGGlobalShader = new Shader();
	if (!mGGlobalShader->Load("Shaders/GBufferGlobal.vert", "Shaders/GBufferGlobal.frag"))
	{
		return false;
	}
	// For the GBuffer, we need to associate each sampler with an index
	//GBufferのサンプラーにindex割りあて
	mGGlobalShader->SetActive();
	//フラグメントシェーダーの3つのsampler2D型のuniformに,
	//バインドされたテクスチャインデックスを割り当て(Gbuffer::SetTexturesActive()を参照)
	mGGlobalShader->SetIntUniform("uGDiffuse", 0);
	mGGlobalShader->SetIntUniform("uGNormal", 1);
	mGGlobalShader->SetIntUniform("uGWorldPos", 2);
	// The view projection is just the sprite one
	//ビュー射影はSpriteと同じ
	mGGlobalShader->SetMatrixUniform("uViewProj", spriteViewProj);
	// The world transform scales to the screen and flips y
	//ワールド変換では全画面へのスケーリングとyの反転(openGLのuv原点が左下のため。普通右上)
	Matrix4 gbufferWorld = Matrix4::CreateScale(mScreenWidth, -mScreenHeight,
												1.0f);
	mGGlobalShader->SetMatrixUniform("uWorldTransform", gbufferWorld);
	
	//Gバッファを元にグローバルライティングを行ったTextureを元に、ローカルライティングを行うためのshaderのロード
	//残る設定していないuniform uViewProj: DrawFromGBuffer(), uPointLight: DrawFromGBuffer()->SetLightUniforms()で行ってます

	// Create a shader for point lights from GBuffer
	mGPointLightShader = new Shader();
	if (!mGPointLightShader->Load("Shaders/BasicMesh.vert",
								  "Shaders/GBufferPointLight.frag"))
	{
		return false;
	}
	// Set sampler indices
	//shaderをactiveに
	mGPointLightShader->SetActive();
	//様々なサンプラーのためのuniform群を設定. Gバッファテクスチャにバインド
	mGPointLightShader->SetIntUniform("uGDiffuse", 0);
	mGPointLightShader->SetIntUniform("uGNormal", 1);
	mGPointLightShader->SetIntUniform("uGWorldPos", 2);
	//画面の幅と高さの設定
	mGPointLightShader->SetVector2Uniform("uScreenDimensions",
		Vector2(mScreenWidth, mScreenHeight));
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

	mSpriteVerts = new VertexArray(vertices, 4, VertexArray::PosNormTex, indices, 6);
}

void Renderer::SetLightUniforms(Shader* shader, const Matrix4& view)
{
	// Camera position is from inverted view
	Matrix4 invView = view;
	invView.Invert();
	shader->SetVectorUniform("uCameraPos", invView.GetTranslation());
	// Ambient light
	shader->SetVectorUniform("uAmbientLight", mAmbientLight);
	// Directional light
	shader->SetVectorUniform("uDirLight.mDirection",
		mDirLight.mDirection);
	shader->SetVectorUniform("uDirLight.mDiffuseColor",
		mDirLight.mDiffuseColor);
	shader->SetVectorUniform("uDirLight.mSpecColor",
		mDirLight.mSpecColor);
}

Vector3 Renderer::Unproject(const Vector3& screenPoint) const
{
	// Convert screenPoint to device coordinates (between -1 and +1)
	Vector3 deviceCoord = screenPoint;
	deviceCoord.x /= (mScreenWidth) * 0.5f;
	deviceCoord.y /= (mScreenHeight) * 0.5f;

	// Transform vector by unprojection matrix
	Matrix4 unprojection = mView * mProjection;
	unprojection.Invert();
	return Vector3::TransformWithPerspDiv(deviceCoord, unprojection);
}

void Renderer::GetScreenDirection(Vector3& outStart, Vector3& outDir) const
{
	// Get start point (in center of screen on near plane)
	Vector3 screenPoint(0.0f, 0.0f, 0.0f);
	outStart = Unproject(screenPoint);
	// Get end point (in center of screen, between near and far)
	screenPoint.z = 0.9f;
	Vector3 end = Unproject(screenPoint);
	// Get direction vector
	outDir = end - outStart;
	outDir.Normalize();
}
