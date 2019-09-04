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

	//OpenGL�̃o�[�W�����A�F�[�x�Ȃǂ̐ݒ�
	//SDL_GL_SetAttribute(�ݒ肷�鑮��, �����l);
	// Set OpenGL attributes
	;	// Use the core OpenGL profile
		//OPENGL���T�|�[�g���郁�C���v���t�@�C���́u�R�A�v�u�݊��v�uES�v��3����
		//�f�X�N�g�b�v�̓R�A����������Ă���A
		//�݊���I������Ɣ񐄏���OpenGL�֐��̌Ăяo�����������
		//OpenGL ES�̓��o�C���J���p
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	// Specify version 3.3
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	// Request a color buffer with 8-bits per RGBA channel
	//r, g, b, a���ꂼ���8bit�̃J���[�o�b�t�@(�K�p�����郁������)���g�p
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	// Enable double buffering
	//�_�u���o�b�t�@�̓K�p
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	// Force OpenGL to use hardware acceleration
	//OpenGL���n�[�h�E�F�A�A�N�Z�����[�V�����t���Ŏ��s = �����_�����O��GPU���g�p
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

	mWindow = SDL_CreateWindow("Game Programming in C++ (Chapter 5)", 100, 100,
		1024, 768, SDL_WINDOW_OPENGL);		//SDL_WINDOW_OPENGL�ō쐬���ꂽ�E�B���h�E��OpenGL���g�p�\��
	if (!mWindow)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}

	// Create an OpenGL context
	//OpenGL�̃R���e�N�X�g(�R���e�L�X�g)�쐬(�R���e�N�X�g�Ƃ͐ݒ�S�̂悤�Ȃ���)
	mContext = SDL_GL_CreateContext(mWindow);

	// Initialize GLEW
	//GLEW�ŏ����� glewInit() => ����ɂ��AVer3.3�Ƃ���ȑO�̃o�[�W�������Ή�����@�\�̑S�����������Ă����
	glewExperimental = GL_TRUE;		//�ꕔ�v���b�g�t�H�[���ŃR���e�N�X�g���g�p����ۂɏo�鏉�����G���[��\�h
	if (glewInit() != GLEW_OK)
	{
		SDL_Log("GLEW�̏������Ɏ��s���܂���");
		return false;
	}
	// On some platforms, GLEW will emit a benign error code,
	// so clear it
	glGetError();		//�ꕔ�Ղ���ƃt�H�[���Ŗ��Q�ȃG���[�R�[�h���o�����߁A�����ŃN���A���Ă���

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
	for (auto pending : mPendingActors)		//���̃A�N�^�[������ꂽ�\���O�̃A�N�^�[�ɑ΂��Ă��A���[���h�s��̌v�Z���s���Ă���
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
2D�Q�[��.. �X�v���C�g��p���ĕ\�����Ă���(�X�v���C�g�̉摜���J���[�o�b�t�@�̂ǂ����փR�s�y���� = �u���b�e�B���O) => �ӂ��݂���̂悤�ȃX�v���C�g�x�[�X�̃Q�[���@�ł͌����I
3D�Q�[��.. 3���������V���~���[�g���A���炩�̕��@��2�����摜�֕��ʉ�����(���̓|���S���̂ق��������I�ŁA�u���b�e�B���O�͔���� => 2D, 3D�Ƃ��Ƀ|���S����p���Ă���Ƃ��낪����)

�|���S���g�p���闝�R
1. �X�P�[���u��(�d������������Ȃ�|���S�������炷)
2. �v�Z�ʂ����Ȃ�

�قƂ�ǂ̃Q�[���̃|���S�����O�p�`
��: �_: �|�C���g, �_���m���Ȃ�����: ���C��, ���łł�����: �|���S��
�Ȃ��O�p�`��?
1. �\������_�͂��ׂē��ꕽ�ʏ�
2. �����Ƃ��V���v���ȃ|���S���ŁA������3�̃|�C���g�ŕ\���ł���
3. �ו����\(�ǂ�Ȃɕ��G�ł��O�p�`�̃|���S���̏W���ŕ\�����Ƃ��ł���)

2D�ł́A�X�v���C�g���ǂ��\�����邩�Ƃ����ƁA
1�̋�`��`��A���̒����摜�f�[�^�̐F�œh��
�Ƃ������ƂɂȂ�
*/
float r = 0;
float g = 0;
float b = 0;

void Game::GenerateOutput()
{
	//�J���[�o�b�t�@�[�̏�����
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
	//�J���[�o�b�t�@�[�̃N���A
	/*
	API��ǂނ�
	glClear�́A�E�B���h�E�̃r�b�g�v���[���̈���A�ȑO��glClearColor�AglClearIndex�AglClearDepth�AglClearStencil�A�����glClearAccum�ɂ���đI�����ꂽ�l�ɐݒ肵�܂��B
	glDrawBuffer���g�p���Ĉ�x�ɕ����̃o�b�t�@��I�����邱�Ƃɂ��A�����̃J���[�o�b�t�@�𓯎��ɃN���A���邱�Ƃ��ł��܂��B
	�Ə����Ă��邽�߁A�D�F�ŃJ���[�o�b�t�@�����������邱�Ƃ��ł���
	*/
	glClear(GL_COLOR_BUFFER_BIT);
	//2D sprite -> RenderCopy(): blitting(�u���b�e�B���O) => �ŋ߂̃O���t�B�b�N�n�[�h�E�F�A�ł͔����
	//3D �|���S�� -> �`����������� => 2D, 3D�ǂ���ł��O���t�B�b�N�����Ƀ|���S�����g�p���Ă���
	//�|���S���̗��_
	/*
	1. �|���S���͑���3D�O���t�B�b�N�Z�@���v�Z�������Ȃ��čς�
	2. �X�P�[���u��
	3. �Ў�ȃn�[�h�ł��A�|���S���������Ȃ�����Γ���
	4. �قƂ�ǂ�3D���f���̓|���S���ŕ\���ł���
	*/
	//�|���S�� = �w�ǂ��u�O�p�`�v(3�̓_ = 3�̒��_��1�̎O�p�`���`��)

	//�X�v���C�g�̒��_�z��I�u�W�F�N�g�ƃV�F�[�_�[���A�N�e�B�u�ɂ���
	mSpriteVerts->SetActive();
	mSpriteShader->SetActive();

	glEnable(GL_BLEND);		//�J���[�o�b�t�@�̃u�����f�B���O�J�n
	glBlendFunc(
		GL_SRC_ALPHA,		//srcFactor -> srcAlpha
		GL_ONE_MINUS_SRC_ALPHA	//dstFactor -> 1 - srcAlpha
	);
	// outputColor = srcFactor*srcColor + dstFactor*dstColor = alphaFactor*srcColor + (1-alphaFactor)*dstColor
	//alphaFactor�͉摜���̂��e�s�N�Z���Ɏ����Ă���
	//�Ȃ��A��̂悤�Ƀu�����f�B���O���Ȃ���
	// outputColor = alphaColor * srcColor 
	// �ƂȂ�AalphaColor = 0�̂Ƃ��A outputColor = (0, 0, 0)�ƂȂ邽�ߍ����`�悳���

	//���ׂĂ�Sprite��`��
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
		//���[�h���s
		return false;
	}
	mSpriteShader->SetActive();

	//�P���ȃr���[�ˉe�s��
	Matrix4 viewProj = Matrix4::CreateSimpleViewProj(1024.f, 768.f);
	mSpriteShader->SetMatrixUniform("uViewProj", viewProj);		//�ŏ�1�񂾂��ݒ肷��Ηǂ�
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
	//���_�V�F�[�_�[: ���_�ɑ΂���1����s�����(����: ���_�����f�[�^)
	//���_�o�b�t�@�[��̌Ăяo���ōςށB�������A���f�������x���`�悷��ꍇ�́A�`�悷��񐔕��Ăяo�����s����(���݂Ɉˑ������A�ʂ̌v�Z�ƂȂ�)
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
	//OpenGL�R���e�L�X�g�̍폜
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
