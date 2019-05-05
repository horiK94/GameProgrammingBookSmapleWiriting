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
	
	if (IMG_Init(IMG_INIT_PNG) == 0)		//�摜�t�@�C���ǂݍ��݂̂��߂̏�����
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
	//�L�����N�^�[�̓��͏���
	character->ProcessAnimKeyBoard(state);
	*/
}

void Game::UpdateGame()
{
	// 16ms�ȉ��̏ꍇ�͑҂�
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
		;

	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;		//�u���C�N�|�C���g�Ƃ���deltaTime���l�ł����Ȃ������̕␳
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
	for (auto pending : mPendingActors)		//foreach�݂����Ȃ��
	{
		mActors.emplace_back(pending);		//actors�ɒǉ����Ă���
	}
	mPendingActors.clear();		//�ǉ����I������̂ŁAclear���Ă���

	// ����actor�̏���
	std::vector<Actor*> deadActors;
	for (auto actor : mActors)
	{
		if (actor->GetState() == Actor::EDead)
		{
			deadActors.emplace_back(actor);		//����ł���actor��deadActors�ɒǉ�
		}
	}

	// Delete dead actors (which removes them from mActors)	mActors������������(�Ƃ����̂��AActor�̃f�X�g���N�^�ŁA���������������Ă��邽��)
	//���̂��߁A���������ŏ��������͂��Ă͂����Ȃ�
	for (auto actor : deadActors)
	{
		delete actor;		//����ł���actor�������Ă���
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

//�Q�[�����[���h�ɓo�ꂷ�邷�ׂẴA�N�^�[���쐬����֐�
void Game::LoadData()
{
	/*
	// Create player's ship
	mShip = new Ship(this);
	mShip->SetPosition(Vector2(100.0f, 384.0f));
	mShip->SetScale(1.5f);

	//�L�����N�^�[�̍쐬
	character = new Character(this);
	character->SetPosition(Vector2(200.0f, 384.0f));
	character->SetScale(1.0f);
	
	//SDL_Image������������΁A���Ƃ�IMG_Load()��SDL_Surface�\���̂Ƀ��[�h�ł���

	// Create actor for the background (this doesn't need a subclass)
	Actor* temp = new Actor(this);
	temp->SetPosition(Vector2(512.0f, 384.0f));		//��ʂ̒��S
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
}

void Game::UnloadData()
{
	// Delete actors
	// Because ~Actor calls RemoveActor, have to use a different style loop
	// removeActor����̂̓f�X�g���N�^�����Adelete�����Ă���
	while (!mActors.empty())
	{
		delete mActors.back();		//�����̗v�f�ւ̎Q�Ƃ��폜����
	}

	// Destroy textures
	for (auto i : mTextures)
	{
		SDL_DestroyTexture(i.second);
	}
	mTextures.clear();
}

//�摜�̃��[�h
SDL_Texture* Game::GetTexture(const std::string& fileName)
{
	//C++03�܂ŁA�k���|�C���^��\�����߂�0���l���e������NULL�}�N�����g�p���Ă����BC++11����́Anullptr�L�[���[�h�Ńk���|�C���^�l��\�����Ƃ𐄏�����B

	SDL_Texture* tex = nullptr;
	// Is the texture already in the map?
	auto iter = mTextures.find(fileName);		//���łɃ��[�h�����摜��������ꂽ�A�z�z��
	if (iter != mTextures.end())
	{
		tex = iter->second;
	}
	else
	{
		// Load from file
		//SDL_Surface: �R�s�[�p�Ɏg����s�N�Z���̏W�܂�̍\����
		SDL_Surface* surf = IMG_Load(fileName.c_str());		//�摜�̓ǂݍ���
		//c_str(): NULL�ŏI�[���ꂽ�����z��̐擪�̃|�C���^��Ԃ�
		/*
		  std::string s("Hello, world!");
		  std::puts(s.c_str());		//Hello, World!

		  s[5] = '\0';
		  std::puts(s.c_str());  // ',' �ȍ~�͏o�͂���Ȃ�(Hello�̂ݏo�͂����)
		*/
		if (!surf)
		{
			SDL_Log("Failed to load texture file %s", fileName.c_str());
			return nullptr;
		}

		// Create texture from surface
		tex = SDL_CreateTextureFromSurface(mRenderer, surf);		//SDL_Surface�\���̂���SDL_Texture�\���̂֕ϊ�
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
	//update���Ȃ�mPendingActors��, update���łȂ��ꍇactors�ɒǉ�
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
	auto iter = std::find(mPendingActors.begin(), mPendingActors.end(), actor);		//auto�Ƃ�c#�ł���var�̂悤�Ȃ��
	//std;;find(): [begin, last)����actor�Ɠ������̂̍ŏ���Ԃ�.�Ȃ�����last��Ԃ�(std::vector::end�͊m�ۂ�����������+1�����ꏊ(�܂�A�l�͖���`))
	if (iter != mPendingActors.end())
	{
		//���������ꍇ
		// Swap to end of vector and pop off (avoid erase copies)
		std::iter_swap(iter, mPendingActors.end() - 1);		//�m�ۂ����������̍Ō�̗v�f��iter����������
		mPendingActors.pop_back();		//�����v�f���폜����
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
	int myDrawOrder = sprite->GetDrawOrder();		//�`�揇���̎擾
	auto iter = mSprites.begin();		//SpriteComponent�̎Q�Ƃ�������ꂽ���I�z��̐擪�̃|�C���^���擾
	for ( ;
		iter != mSprites.end();
		++iter)
	{
		//��; �`�揇�����Ⴂ�قǉ����ɒu�����
		if (myDrawOrder < (*iter)->GetDrawOrder())
		{
			break;
		}
	}

	// Inserts element before position of iterator
	mSprites.insert(iter, sprite);		//�}��
	//insert(�ǂ̗v�f�̑O�ɑ}�����邩, ����}�����邩)
}

void Game::RemoveSprite(SpriteComponent* sprite)
{
	// (We can't swap because it ruins ordering)
	auto iter = std::find(mSprites.begin(), mSprites.end(), sprite);
	mSprites.erase(iter);
}
