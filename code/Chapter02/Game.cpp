// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Game.h"
#include "SDL/SDL_image.h"
#include <algorithm>
#include "Actor.h"
#include "SpriteComponent.h"
#include "Ship.h"
#include "BGSpriteComponent.h"
#include "Character.h"
#include "TileMapComponent.h"

Game::Game()
:mWindow(nullptr)
,mRenderer(nullptr)
,mIsRunning(true)
,mUpdatingActors(false)
{
	
}

bool Game::Initialize()
{
	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}
	
	mWindow = SDL_CreateWindow("Game Programming in C++ (Chapter 2)", 100, 100, 1024, 768, 0);
	if (!mWindow)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}
	
	mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!mRenderer)
	{
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return false;
	}
	
	if (IMG_Init(IMG_INIT_PNG) == 0)		//画像ファイル読み込みのための初期化
	{
		SDL_Log("Unable to initialize SDL_image: %s", SDL_GetError());
		return false;
	}

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
	
	const Uint8* state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}
	/*
	// Process ship input
	mShip->ProcessKeyboard(state);
	//キャラクターの入力処理
	character->ProcessAnimKeyBoard(state);
	*/
}

void Game::UpdateGame()
{
	// 16ms以下の場合は待つ
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
		;

	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;		//ブレイクポイントとかでdeltaTimeが値でかくなった時の補正
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
	for (auto pending : mPendingActors)		//foreachみたいなやつ
	{
		mActors.emplace_back(pending);		//actorsに追加していく
	}
	mPendingActors.clear();		//追加し終わったので、clearしておく

	// 死んだactorの消去
	std::vector<Actor*> deadActors;
	for (auto actor : mActors)
	{
		if (actor->GetState() == Actor::EDead)
		{
			deadActors.emplace_back(actor);		//死んでいるactorをdeadActorsに追加
		}
	}

	// Delete dead actors (which removes them from mActors)	mActorsから消去される(というのも、Actorのデストラクタで、消す処理が走っているため)
	//そのため、こっち側で消す処理はしてはいけない
	for (auto actor : deadActors)
	{
		delete actor;		//死んでいるactorを消していく
	}
}

void Game::GenerateOutput()
{
	SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255);
	SDL_RenderClear(mRenderer);
	
	// Draw all sprite components
	for (auto sprite : mSprites)
	{
		sprite->Draw(mRenderer);
	}

	SDL_RenderPresent(mRenderer);
}

//ゲームワールドに登場するすべてのアクターを作成する関数
void Game::LoadData()
{
	/*
	// Create player's ship
	mShip = new Ship(this);
	mShip->SetPosition(Vector2(100.0f, 384.0f));
	mShip->SetScale(1.5f);

	//キャラクターの作成
	character = new Character(this);
	character->SetPosition(Vector2(200.0f, 384.0f));
	character->SetScale(1.0f);
	
	//SDL_Imageを初期化すれば、あとはIMG_Load()でSDL_Surface構造体にロードできる

	// Create actor for the background (this doesn't need a subclass)
	Actor* temp = new Actor(this);
	temp->SetPosition(Vector2(512.0f, 384.0f));		//画面の中心
	// Create the "far back" background
	BGSpriteComponent* bg = new BGSpriteComponent(temp);
	bg->SetScreenSize(Vector2(1024.0f, 768.0f));
	std::vector<SDL_Texture*> bgtexs = {
		GetTexture("Assets/Farback01.png"),
		GetTexture("Assets/Farback02.png")
	};
	bg->SetBGTextures(bgtexs);
	bg->SetScrollSpeed(-100.0f);
	// Create the closer background
	bg = new BGSpriteComponent(temp, 50);
	bg->SetScreenSize(Vector2(1024.0f, 768.0f));
	bgtexs = {
		GetTexture("Assets/Stars.png"),
		GetTexture("Assets/Stars.png")
	};
	bg->SetBGTextures(bgtexs);
	bg->SetScrollSpeed(-200.0f);
	*/
	Actor* bg = new Actor(this);
	bg->SetPosition(Vector2(512.0f, 384.0f));
	SDL_Texture* tileTexture = GetTexture("Assets/Tiles.png");

	Vector2 screenSize(1024.0f, 768.0f);

	//以下はfor文でやったほうがホントは良いと思われる
	TileMapComponent* tm1 = new TileMapComponent(bg, 150);
	tm1->SetTileSet(tileTexture, screenSize, Game::TILE_WIDTH_NUM, Game::TILE_HEIGHT_NUM);
	tm1->SetTileData("Assets/MapLayer1.csv");
	bg->AddComponent(tm1);
	
	TileMapComponent* tm2 = new TileMapComponent(bg, 100);
	tm2->SetTileSet(tileTexture, screenSize, Game::TILE_WIDTH_NUM, Game::TILE_HEIGHT_NUM);
	tm2->SetTileData("Assets/MapLayer2.csv");
	bg->AddComponent(tm2);

	TileMapComponent* tm3 = new TileMapComponent(bg, 50);
	tm3->SetTileSet(tileTexture, screenSize, Game::TILE_WIDTH_NUM, Game::TILE_HEIGHT_NUM);
	tm3->SetTileData("Assets/MapLayer3.csv");
	bg->AddComponent(tm3);
}

void Game::UnloadData()
{
	// Delete actors
	// Because ~Actor calls RemoveActor, have to use a different style loop
	// removeActorするのはデストラクタ時より、deleteをしている
	while (!mActors.empty())
	{
		delete mActors.back();		//末尾の要素への参照を削除する
	}

	// Destroy textures
	for (auto i : mTextures)
	{
		SDL_DestroyTexture(i.second);
	}
	mTextures.clear();
}

//画像のロード
SDL_Texture* Game::GetTexture(const std::string& fileName)
{
	//C++03まで、ヌルポインタを表すために0数値リテラルやNULLマクロを使用していた。C++11からは、nullptrキーワードでヌルポインタ値を表すことを推奨する。

	SDL_Texture* tex = nullptr;
	// Is the texture already in the map?
	auto iter = mTextures.find(fileName);		//すでにロードした画像が代入された連想配列
	if (iter != mTextures.end())
	{
		tex = iter->second;
	}
	else
	{
		// Load from file
		//SDL_Surface: コピー用に使われるピクセルの集まりの構造体
		SDL_Surface* surf = IMG_Load(fileName.c_str());		//画像の読み込み
		//c_str(): NULLで終端された文字配列の先頭のポインタを返す
		/*
		  std::string s("Hello, world!");
		  std::puts(s.c_str());		//Hello, World!

		  s[5] = '\0';
		  std::puts(s.c_str());  // ',' 以降は出力されない(Helloのみ出力される)
		*/
		if (!surf)
		{
			SDL_Log("Failed to load texture file %s", fileName.c_str());
			return nullptr;
		}

		// Create texture from surface
		tex = SDL_CreateTextureFromSurface(mRenderer, surf);		//SDL_Surface構造体からSDL_Texture構造体へ変換
		SDL_FreeSurface(surf);
		if (!tex)
		{
			SDL_Log("Failed to convert surface to texture for %s", fileName.c_str());
			return nullptr;
		}

		mTextures.emplace(fileName.c_str(), tex);
	}
	return tex;
}

void Game::Shutdown()
{
	UnloadData();
	IMG_Quit();
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}

void Game::AddActor(Actor* actor)
{
	// If we're updating actors, need to add to pending
	//update中ならmPendingActorsに, update中でない場合actorsに追加
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
	auto iter = std::find(mPendingActors.begin(), mPendingActors.end(), actor);		//autoとはc#でいうvarのようなやつ
	//std;;find(): [begin, last)内にactorと同じものの最初を返す.なかったlastを返す(std::vector::endは確保したメモリに+1した場所(つまり、値は未定義))
	if (iter != mPendingActors.end())
	{
		//見つかった場合
		// Swap to end of vector and pop off (avoid erase copies)
		std::iter_swap(iter, mPendingActors.end() - 1);		//確保したメモリの最後の要素とiterを交換する
		mPendingActors.pop_back();		//末尾要素を削除する
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
	int myDrawOrder = sprite->GetDrawOrder();		//描画順序の取得
	auto iter = mSprites.begin();		//SpriteComponentの参照が代入された動的配列の先頭のポインタを取得
	for ( ;
		iter != mSprites.end();
		++iter)
	{
		//注; 描画順序が低いほど遠くに置かれる
		if (myDrawOrder < (*iter)->GetDrawOrder())
		{
			break;
		}
	}

	// Inserts element before position of iterator
	mSprites.insert(iter, sprite);		//挿入
	//insert(どの要素の前に挿入するか, 何を挿入するか)
}

void Game::RemoveSprite(SpriteComponent* sprite)
{
	// (We can't swap because it ruins ordering)
	auto iter = std::find(mSprites.begin(), mSprites.end(), sprite);
	mSprites.erase(iter);
}