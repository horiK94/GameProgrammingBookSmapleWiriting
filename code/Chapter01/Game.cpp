// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Game.h"

const int thickness = 15;
const float paddleH = 100.0f;

Game::Game()
:mWindow(nullptr)
,mRenderer(nullptr)
,mTicksCount(0)
,mIsRunning(true)
,mPaddleDir(0)
{
	
}

bool Game::Initialize()
{
	// Initialize SDL
	int sdlResult = SDL_Init(SDL_INIT_VIDEO);		//SDL�̏�����
	if (sdlResult != 0)
	{
		//���������s
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());		//SDL_Log��c#�ł���COnsole.Log()�I�Ȃ��
		return false;
	}
	
	// Create an SDL Window
	mWindow = SDL_CreateWindow(
		"Game Programming in C++ (Chapter 1)", // Window title		(���{��̓o�O��̂Œ���)
		100,	// Top left x-coordinate of window
		100,	// Top left y-coordinate of window
		1024,	// Width of window
		768,	// Height of window
		0		// Flags (0 for no flags set)
	);

	if (!mWindow)
	{

		// window�̍쐬���s
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}
	
	//// Create SDL renderer
	mRenderer = SDL_CreateRenderer(
		mWindow, // Window to create renderer for(�쐬���郌���_���[�̕`��ΏۂƂȂ�E�B���h�E)
		-1,		 // Usually -1�i�O���t�B�b�N�h���C�o�[. �E�B���h�E1�Ȃ̂Ńf�t�H���g��-1��I�Ԃ��Ƃ�SDL�����߂Ă����j
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);
	//SDL_RENDERER_ACCELERATED..�O���t�B�b�N�X�n�[�h�E�F�A�̋@�\���d�l
	//SDL_RENDERER_PRESENTVSYNC..����������p����

	if (!mRenderer)
	{
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return false;
	}
	//
	mPaddlePos.x = 10.0f;
	mPaddlePos.y = 768.0f/2.0f;
	mBallPos.x = 1024.0f/2.0f;
	mBallPos.y = 768.0f/2.0f;
	mBallVel.x = -200.0f;
	mBallVel.y = 235.0f;
	return true;
}

void Game::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();		//���͎�t
		UpdateGame();		//�X�V
		GenerateOutput();	//�`��
	}
}

void Game::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))		//�C�x���g�������true��
	{
		//�����̃C�x���g�������Ă���\��������̂ŁA���ׂẴC�x���g���[�v��������K�v������
		switch (event.type)
		{
			// If we get an SDL_QUIT event, end loop
			case SDL_QUIT:
				mIsRunning = false;
				break;
		}
	}

	// Get state of keyboard
	const Uint8* state = SDL_GetKeyboardState(NULL);		//�L�[�{�[�h�S�̂̏�Ԃ��������z��ւ̃|�C���^��Ԃ�
	// If escape is pressed, also end loop
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}
	
	/*
	// Update paddle direction based on W/S keys
	mPaddleDir = 0;
	if (state[SDL_SCANCODE_W])
	{
		mPaddleDir -= 1;
	}
	if (state[SDL_SCANCODE_S])
	{
		mPaddleDir += 1;
	}
	*/
}

void Game::UpdateGame()
{
	// Wait until 16ms has elapsed since last frame
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
		;

	//16ms�߂��Ă�����ȉ������s
	// Delta time is the difference in ticks from last frame
	// (converted to seconds)
	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	
	// Clamp maximum delta time value
	//�u���C�N�|�C���g�Ƃ���50ms�����Ă�����A50ms�Ƃ��Ĉ���
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}

	//���t���[���̂��߁A���݂̌o�ߎ��Ԃ�ۑ�
	// Update tick counts (for next frame)
	mTicksCount = SDL_GetTicks();
	
	//�o�h���𑀍쎞
	// Update paddle position based on direction
	if (mPaddleDir != 0)
	{
		mPaddlePos.y += mPaddleDir * 300.0f * deltaTime;
		// Make sure paddle doesn't move off screen!
		if (mPaddlePos.y < (paddleH/2.0f + thickness))
		{
			mPaddlePos.y = paddleH/2.0f + thickness;
		}
		else if (mPaddlePos.y > (768.0f - paddleH/2.0f - thickness))
		{
			mPaddlePos.y = 768.0f - paddleH/2.0f - thickness;
		}
	}
	
	// Update ball position based on ball velocity
	mBallPos.x += mBallVel.x * deltaTime;
	mBallPos.y += mBallVel.y * deltaTime;
	
	// Bounce if needed
	// Did we intersect with the paddle?
	float diff = mPaddlePos.y - mBallPos.y;
	// Take absolute value of difference
	diff = (diff > 0.0f) ? diff : -diff;
	if (
		// Our y-difference is small enough
		diff <= paddleH / 2.0f &&
		// We are in the correct x-position
		mBallPos.x <= 25.0f && mBallPos.x >= 20.0f &&
		// The ball is moving to the left
		mBallVel.x < 0.0f)
	{
		mBallVel.x *= -1.0f;
	}
	// Did the ball go off the screen? (if so, end game)
	else if (mBallPos.x <= 0.0f)
	{
		mIsRunning = false;
	}
	// Did the ball collide with the right wall?
	else if (mBallPos.x >= (1024.0f - thickness) && mBallVel.x > 0.0f)
	{
		mBallVel.x *= -1.0f;
	}
	
	// Did the ball collide with the top wall?
	if (mBallPos.y <= thickness && mBallVel.y < 0.0f)
	{
		mBallVel.y *= -1;
	}
	// Did the ball collide with the bottom wall?
	else if (mBallPos.y >= (768 - thickness) &&
		mBallVel.y > 0.0f)
	{
		mBallVel.y *= -1;
	}
}

void Game::GenerateOutput()
{	
	//�o�b�N�o�b�t�@��P�F�œh��Ԃ�(�O�ɕ`�悵�����e�̏�ɕ`�悵�Ȃ�����)
	//�����Ń����_���[�ւ̃|�C���^���󂯎��
	// Set draw color to blue
	SDL_SetRenderDrawColor(
		mRenderer,
		0,		// R
		0,		// G 
		255,	// B
		255		// A
	);
	
	// Clear back buffer
	SDL_RenderClear(mRenderer);		//���݂̃����_�[�^�[�Q�b�g��F�œh��Ԃ�(�Ō��SDL_RenderPreset()���Ă΂Ȃ��ƃt�����g�o�b�t�@�ƃo�b�N�o�b�t�@���������ꂸ�^�����ȉ�ʂɂȂ��Ă��܂�)



	//�`��̓o�b�N�o�b�t�@���N���A�������ƁA�t�����g�o�b�t�@�ƌ�������܂łɍs��

	// Draw walls
	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);		//mRendere�ɔ��̎Q�Ƃ�n��
	
	// Draw top wall
	SDL_Rect wall{		//SDL_Rect�̈����͂��ׂ�int�^�Ŏw�肳��Ă���
		0,			// Top left x
		0,			// Top left y
		1024,		// Width
		thickness	// Height
	};
	SDL_RenderFillRect(mRenderer, &wall);		//�h��Ԃ��ꂽ�����`�̕`��

	// Draw bottom wall
	wall.y = 768 - thickness;		//������768�Ŏw�肵�Ă��邽��
	SDL_RenderFillRect(mRenderer, &wall);
	
	// Draw right wall
	wall.x = 1024 - thickness;
	wall.y = 0;
	wall.w = thickness;
	wall.h = 1024;
	SDL_RenderFillRect(mRenderer, &wall);

	// Draw paddle(�p�h���̕`��)
	SDL_Rect paddle{		//�p�h���͓��������߁AUpdateGame()�ňʒu��ݒ肵�����ŕ`�悵�Ă���(�ʒu��Vector2�\���̂̕ϐ�mPaddlePos���g�p�B����̓p�h���̍��^�񒆂̈ʒu��\���Ă���)
		static_cast<int>(mPaddlePos.x),		//int�^�ւ̕ύX.SDL_Rect�̈����͂��ׂ�int�^�̂���.static_cast�̈Öق̌^�ϊ����s��
		static_cast<int>(mPaddlePos.y - paddleH/2),
		thickness,
		static_cast<int>(paddleH)
	};
	SDL_RenderFillRect(mRenderer, &paddle);
	
	// Draw ball
	SDL_Rect ball{			////�{�[���͓������߁AUpdateGame()�ňʒu��ݒ肵�����ŕ`�悵�Ă���(�ʒu��Vector2�\���̂̕ϐ�mBallPos���g�p�B����̓{�[���̒��S�̈ʒu��\���Ă���)
		static_cast<int>(mBallPos.x - thickness/2),
		static_cast<int>(mBallPos.y - thickness/2),
		thickness,
		thickness
	};
	SDL_RenderFillRect(mRenderer, &ball);
	
	// Swap front buffer and back buffer
	//����Ă΂Ȃ��Ɛ^�����Ȃ܂�
	SDL_RenderPresent(mRenderer);
}

void Game::Shutdown()
{
	SDL_DestroyRenderer(mRenderer);		//�����_���[�̏I��
	SDL_DestroyWindow(mWindow);		//�E�B���h�E�̏I��
	SDL_Quit();		//SDL�̏I��
}
