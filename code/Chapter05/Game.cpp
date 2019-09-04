// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Game.h"
#include <GL/glew.h>
#include "Texture.h"
#include "VertexArray.h"
#include "Shader.h"
#include <algorithm>
#include "Actor.h"
#include "SpriteComponent.h"
#include "Actor.h"
#include "Ship.h"
#include "Asteroid.h"
#include "Random.h"
#include <random>

Game::Game()
	:mWindow(nullptr)
	, mSpriteShader(nullptr)
	, mIsRunning(true)
	, mUpdatingActors(false)
{

}

bool Game::Initialize()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}

	//OpenGLのバージョン、色深度などの設定
	//SDL_GL_SetAttribute(設定する属性, 属性値);
	// Set OpenGL attributes
	;	// Use the core OpenGL profile
		//OPENGLがサポートするメインプロファイルは「コア」「互換」「ES」の3つあり
		//デスクトップはコアが推奨されており、
		//互換を選択すると非推奨のOpenGL関数の呼び出しが許可される
		//OpenGL ESはモバイル開発用
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	// Specify version 3.3
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	// Request a color buffer with 8-bits per RGBA channel
	//r, g, b, aそれぞれに8bitのカラーバッファ(適用させるメモリ量)を使用
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	// Enable double buffering
	//ダブルバッファの適用
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	// Force OpenGL to use hardware acceleration
	//OpenGLをハードウェアアクセラレーション付きで実行 = レンダリングにGPUを使用
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

	mWindow = SDL_CreateWindow("Game Programming in C++ (Chapter 5)", 100, 100,
		1024, 768, SDL_WINDOW_OPENGL);		//SDL_WINDOW_OPENGLで作成されたウィンドウでOpenGLが使用可能に
	if (!mWindow)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}

	// Create an OpenGL context
	//OpenGLのコンテクスト(コンテキスト)作成(コンテクストとは設定郡のようなもの)
	mContext = SDL_GL_CreateContext(mWindow);

	// Initialize GLEW
	//GLEWで初期化 glewInit() => これにより、Ver3.3とそれ以前のバージョンが対応する機能の全初期化をしてくれる
	glewExperimental = GL_TRUE;		//一部プラットフォームでコンテクストを使用する際に出る初期化エラーを予防
	if (glewInit() != GLEW_OK)
	{
		SDL_Log("GLEWの初期化に失敗しました");
		return false;
	}
	// On some platforms, GLEW will emit a benign error code,
	// so clear it
	glGetError();		//一部ぷらっとフォームで無害なエラーコードを出すため、ここでクリアしている

	// Make sure we can create/compile shaders
	if (!LoadShaders())
	{
		SDL_Log("Failed to load shaders.");
		return false;
	}

	// Create quad for drawing sprites
	CreateSpriteVerts();

	LoadData();

	mTicksCount = SDL_GetTicks();

	return true;
}

void Game::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			mIsRunning = false;
			break;
		}
	}

	const Uint8* keyState = SDL_GetKeyboardState(NULL);
	if (keyState[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}

	mUpdatingActors = true;
	for (auto actor : mActors)
	{
		actor->ProcessInput(keyState);
	}
	mUpdatingActors = false;
}

float changeTime = 3.0f;
float time = changeTime;

void Game::UpdateGame()
{
	// Compute delta time
	// Wait until 16ms has elapsed since last frame
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
		;

	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}
	mTicksCount = SDL_GetTicks();

	// Update all actors
	mUpdatingActors = true;
	for (auto actor : mActors)
	{
		actor->Update(deltaTime);
	}
	mUpdatingActors = false;

	// Move any pending actors to mActors
	for (auto pending : mPendingActors)		//他のアクターから作られた表示前のアクターに対しても、ワールド行列の計算を行っておく
	{
		/*pending->ComputeWorldTransform();*/
		pending->ComputeWorldTransform();
		mActors.emplace_back(pending);
	}
	mPendingActors.clear();

	// Add any dead actors to a temp vector
	std::vector<Actor*> deadActors;
	for (auto actor : mActors)
	{
		if (actor->GetState() == Actor::EDead)
		{
			deadActors.emplace_back(actor);
		}
	}

	// Delete dead actors (which removes them from mActors)
	for (auto actor : deadActors)
	{
		delete actor;
	}

	time += deltaTime;
}

/*
2Dゲーム.. スプライトを用いて表示していく(スプライトの画像をカラーバッファのどこかへコピペする = ブリッティング) => ふぁみこんのようなスプライトベースのゲーム機では効率的
3Dゲーム.. 3次元環境をシュミレートし、何らかの方法で2次元画像へ平面化する(今はポリゴンのほうが効率的で、ブリッティングは非効率 => 2D, 3Dともにポリゴンを用いているところが多い)

ポリゴン使用する理由
1. スケーラブル(重い処理が難しいならポリゴン数減らす)
2. 計算量が少ない

ほとんどのゲームのポリゴンが三角形
注: 点: ポイント, 点同士をつなげた線: ライン, 線でできた面: ポリゴン
なぜ三角形か?
1. 構成する点はすべて同一平面上
2. もっともシンプルなポリゴンで、たった3つのポイントで表現できる
3. 細分割可能(どんなに複雑でも三角形のポリゴンの集合で表すことができる)

2Dでは、スプライトをどう表現するかというと、
1個の矩形を描画、その中を画像データの色で塗る
ということになる
*/
float r = 0;
float g = 0;
float b = 0;

void Game::GenerateOutput()
{
	//カラーバッファーの初期化
	if (time >= changeTime)
	{
		std::random_device rd;
		std::mt19937 mt(rd());
		std::uniform_real_distribution<float> rand(0.0, 1.0);
		r = rand(mt);
		g = rand(mt);
		b = rand(mt);
		SDL_Log("r: %f, g: %f, b: %f", r, g, b);
		time = 0;
	}

	glClearColor(Math::Lerp(r, 0, time / changeTime), Math::Lerp(r, 0, time / changeTime), Math::Lerp(r, 0, time / changeTime), 1.0f);
	//glClearColor(0.86f, 0.86f, 0.86f, 1.0f);
	//カラーバッファーのクリア
	/*
	APIを読むと
	glClearは、ウィンドウのビットプレーン領域を、以前にglClearColor、glClearIndex、glClearDepth、glClearStencil、およびglClearAccumによって選択された値に設定します。
	glDrawBufferを使用して一度に複数のバッファを選択することにより、複数のカラーバッファを同時にクリアすることができます。
	と書いてあるため、灰色でカラーバッファを初期化することができる
	*/
	glClear(GL_COLOR_BUFFER_BIT);
	//2D sprite -> RenderCopy(): blitting(ブリッティング) => 最近のグラフィックハードウェアでは非効率
	//3D ポリゴン -> 描画効率が高い => 2D, 3Dどちらでもグラフィック処理にポリゴンを使用している
	//ポリゴンの利点
	/*
	1. ポリゴンは他の3Dグラフィック技法より計算数が少なくて済む
	2. スケーラブル
	3. ひ弱なハードでも、ポリゴン数を少なくすれば動く
	4. ほとんどの3Dモデルはポリゴンで表現できる
	*/
	//ポリゴン = 殆どが「三角形」(3つの点 = 3つの頂点で1つの三角形を形成)

	//スプライトの頂点配列オブジェクトとシェーダーをアクティブにする
	mSpriteVerts->SetActive();
	mSpriteShader->SetActive();

	glEnable(GL_BLEND);		//カラーバッファのブレンディング開始
	glBlendFunc(
		GL_SRC_ALPHA,		//srcFactor -> srcAlpha
		GL_ONE_MINUS_SRC_ALPHA	//dstFactor -> 1 - srcAlpha
	);
	// outputColor = srcFactor*srcColor + dstFactor*dstColor = alphaFactor*srcColor + (1-alphaFactor)*dstColor
	//alphaFactorは画像自体が各ピクセルに持っている
	//なお、上のようにブレンディングしないと
	// outputColor = alphaColor * srcColor 
	// となり、alphaColor = 0のとき、 outputColor = (0, 0, 0)となるため黒が描画される

	//すべてのSpriteを描画
	for(auto sprite : mSprites)
	{
		sprite->Draw(mSpriteShader);
	}

	/*
	// Draw all sprite components
	// Enable alpha blending on the color buffer
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Set shader/vao as active
	mSpriteShader->SetActive();
	mSpriteVerts->SetActive();
	for (auto sprite : mSprites)
	{
		sprite->Draw(mSpriteShader);
	}
	*/
	// Swap the buffers
	SDL_GL_SwapWindow(mWindow);
}

bool Game::LoadShaders()
{
	//mSpriteShader = new Shader();
	//if (!mSpriteShader->Load("Shaders/Sprite.vert", "Shaders/Sprite.frag"))
	//{
	//	return false;
	//}

	//mSpriteShader->SetActive();
	//// Set the view-projection matrix
	//Matrix4 viewProj = Matrix4::CreateSimpleViewProj(1024.f, 768.f);
	//mSpriteShader->SetMatrixUniform("uViewProj", viewProj);
	//return true;
	mSpriteShader = new Shader();
	if(!mSpriteShader->Load("Shaders/Sprite.vert", "Shaders/Sprite.frag"))
	{
		//ロード失敗
		return false;
	}
	mSpriteShader->SetActive();

	//単純なビュー射影行列
	Matrix4 viewProj = Matrix4::CreateSimpleViewProj(1024.f, 768.f);
	mSpriteShader->SetMatrixUniform("uViewProj", viewProj);		//最初1回だけ設定すれば良い
	return true;
}

void Game::CreateSpriteVerts()
{
	float vertices[] = {
		-0.5f,  0.5f, 0.f, 0.f, 0.f, // top left
		 0.5f,  0.5f, 0.f, 1.f, 0.f, // top right
		 0.5f, -0.5f, 0.f, 1.f, 1.f, // bottom right
		-0.5f, -0.5f, 0.f, 0.f, 1.f  // bottom left
	};
	//float vertices[] = {
	//-0.5f,  0.5f, 0.f, // top left
	// 0.5f,  0.5f, 0.f, // top right
	// 0.5f, -0.5f, 0.f, // bottom right
	//-0.5f, -0.5f, 0.f, // bottom left
	//};

	unsigned int indexBuffer[] = {
		0, 1, 2,
		2, 3, 0
	};

	mSpriteVerts = new VertexArray(vertices, 4, indexBuffer, 6);
	//頂点シェーダー: 頂点に対して1回実行される(入力: 頂点属性データ)
	//頂点バッファー回の呼び出しで済む。ただし、モデルを何度も描画する場合は、描画する回数分呼び出しが行われる(相互に依存せず、個別の計算となる)
}

void Game::LoadData()
{
	// Create player's ship
	mShip = new Ship(this);
	mShip->SetRotation(Math::PiOver2);

	// Create asteroids
	const int numAsteroids = 20;
	for (int i = 0; i < numAsteroids; i++)
	{
		new Asteroid(this);
	}
}

void Game::UnloadData()
{
	// Delete actors
	// Because ~Actor calls RemoveActor, have to use a different style loop
	while (!mActors.empty())
	{
		delete mActors.back();
	}

	// Destroy textures
	for (auto i : mTextures)
	{
		i.second->Unload();
		delete i.second;
	}
	mTextures.clear();
}

Texture* Game::GetTexture(const std::string& fileName)
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

void Game::AddAsteroid(Asteroid* ast)
{
	mAsteroids.emplace_back(ast);
}

void Game::RemoveAsteroid(Asteroid* ast)
{
	auto iter = std::find(mAsteroids.begin(),
		mAsteroids.end(), ast);
	if (iter != mAsteroids.end())
	{
		mAsteroids.erase(iter);
	}
}

void Game::Shutdown()
{
	UnloadData();
	delete mSpriteVerts;
	mSpriteShader->Unload();
	delete mSpriteShader;
	//OpenGLコンテキストの削除
	SDL_GL_DeleteContext(mContext);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}

void Game::AddActor(Actor* actor)
{
	// If we're updating actors, need to add to pending
	if (mUpdatingActors)
	{
		mPendingActors.emplace_back(actor);
	}
	else
	{
		mActors.emplace_back(actor);
	}
}

void Game::RemoveActor(Actor* actor)
{
	// Is it in pending actors?
	auto iter = std::find(mPendingActors.begin(), mPendingActors.end(), actor);
	if (iter != mPendingActors.end())
	{
		// Swap to end of vector and pop off (avoid erase copies)
		std::iter_swap(iter, mPendingActors.end() - 1);
		mPendingActors.pop_back();
	}

	// Is it in actors?
	iter = std::find(mActors.begin(), mActors.end(), actor);
	if (iter != mActors.end())
	{
		// Swap to end of vector and pop off (avoid erase copies)
		std::iter_swap(iter, mActors.end() - 1);
		mActors.pop_back();
	}
}

void Game::AddSprite(SpriteComponent* sprite)
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

void Game::RemoveSprite(SpriteComponent* sprite)
{
	auto iter = std::find(mSprites.begin(), mSprites.end(), sprite);
	mSprites.erase(iter);
}
