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
const int ballNum = 2;
const float ballsPosX[] = { 1024.0f / 2.0f,  1024.0f / 2.0f };
const float ballsPosY[] = { 768.0f / 2.0f, 768.0f / 2.0f };
const float ballsVecX[] = { -200.0f, 200.0f };
const float ballsVecY[] = { 235.0f , 235.0f };

Game::Game()
	:mWindow(nullptr)
	, mRenderer(nullptr)
	, mTicksCount(0)
	, mIsRunning(true)
	, mPaddleDir(0)
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
	//������
	leftPaddlePos.x = 10.0f;
	leftPaddlePos.y = 768.0f / 2.0f;
	rightPaddlePos.x = 1014.0f;
	rightPaddlePos.y = 768.0f / 2.0f;

	balls = std::vector<Ball>(ballNum);
	for (int i = 0; i < ballNum; i++)
	{
		Ball ball = balls[i];
		ball.position.x = ballsPosX[i];
		ball.position.y = ballsPosY[i];
		ball.motionVector.x = ballsVecX[i];
		ball.motionVector.y = ballsVecY[i];

		balls[i] = ball;
	}

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

	rightPaddleDir = 0;
	//�E�p�h���̓��͏���
	if (state[SDL_SCANCODE_I])
	{
		rightPaddleDir -= 1;
	}
	if (state[SDL_SCANCODE_K])
	{
		rightPaddleDir += 1;
	}
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
		leftPaddlePos.y += mPaddleDir * 300.0f * deltaTime;
		// Make sure paddle doesn't move off screen!
		if (leftPaddlePos.y < (paddleH / 2.0f + thickness))
		{
			leftPaddlePos.y = paddleH / 2.0f + thickness;
		}
		else if (leftPaddlePos.y > (768.0f - paddleH / 2.0f - thickness))
		{
			leftPaddlePos.y = 768.0f - (paddleH / 2.0f) - thickness;
		}
	}

	//�E�p�h���̑���
	if (rightPaddleDir != 0)
	{
		//0�ȊO�̂Ƃ� = �ړ��������Ƃ�
		rightPaddlePos.y += rightPaddleDir * 300.0f * deltaTime;
		if (rightPaddlePos.y - paddleH / 2 - thickness < 0)
		{
			rightPaddlePos.y = thickness + paddleH / 2;
		}
		else if (rightPaddlePos.y + paddleH / 2 + thickness > 768)
		{
			rightPaddlePos.y = 768 - (paddleH / 2.0f) - thickness;
		}
	}

	//ball�̍X�V
	for (int i = 0; i < balls.size(); i++)
	{
		Ball ball = balls[i];
		ball.position.x += ball.motionVector.x * deltaTime;
		ball.position.y += ball.motionVector.y * deltaTime;

		//�p�h���ƃ{�[���̈ʒu��(�ʒu�֌W�𒲂ׂ�)
		float diffLeft = leftPaddlePos.y - ball.position.y;
		//���̐�Βl�����߂�
		diffLeft = (diffLeft > 0.0f) ? diffLeft : -diffLeft;
		if (
			//�����p�h���̑傫���̒��ɓ����Ă���
			diffLeft <= paddleH / 2.0f &&
			// �{�[���������x�̂����ɂ��邩
			ball.position.x <= 25.0f && ball.position.x >= 20.0f &&
			// �{�[���̃x�N�g�����p�h���Ɍ�����������
			ball.motionVector.x < 0.0f)
		{
			ball.motionVector.x *= -1.0f;
		}

		float diffRight = rightPaddlePos.y - ball.position.y;
		diffRight = diffRight > 0 ? diffRight : -diffRight;
		if (diffRight <= paddleH / 2
			&& ball.position.x >= rightPaddlePos.x - 25 && ball.position.x <= rightPaddlePos.x - 10
			&& ball.motionVector.x > 0)
		{
			ball.motionVector.x *= -1;
		}
		//�ǂ��炩�̃`�[���̕����̂��߃Q�[���I��
		if (ball.position.x <= 0.0f || ball.position.x >= 1024)
		{
			mIsRunning = false;
		}

		//��̕ǂƂ̏Փ�
		if (ball.position.y <= thickness && ball.position.y < 0.0f)
		{
			ball.motionVector.y *= -1;
		}
		//���̕ǂƂ̏Փ�
		else if (ball.position.y >= (768 - thickness) &&
			ball.motionVector.y > 0.0f)
		{
			ball.motionVector.y *= -1;
		}

		// Ball�͍\���̂̂��߁A�Ō�Ɍ��ʂ���꒼��
		balls[i] = ball;
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
	/*
	wall.x = 1024 - thickness;
	wall.y = 0;
	wall.w = thickness;
	wall.h = 1024;
	SDL_RenderFillRect(mRenderer, &wall);
	*/

	// Draw paddle(���p�h���̕`��)
	SDL_Rect leftPaddle{		//�p�h���͓��������߁AUpdateGame()�ňʒu��ݒ肵�����ŕ`�悵�Ă���(�ʒu��Vector2�\���̂̕ϐ�leftPaddlePos���g�p�B����̓p�h���̍��^�񒆂̈ʒu��\���Ă���)
		static_cast<int>(leftPaddlePos.x),		//int�^�ւ̕ύX.SDL_Rect�̈����͂��ׂ�int�^�̂���.static_cast�̈Öق̌^�ϊ����s��
		static_cast<int>(leftPaddlePos.y - paddleH / 2),
		thickness,
		static_cast<int>(paddleH)
	};
	SDL_RenderFillRect(mRenderer, &leftPaddle);

	// �E�p�h���̕`��
	SDL_Rect rightPaddle{
		static_cast<int>(rightPaddlePos.x - thickness),
		static_cast<int>(rightPaddlePos.y - paddleH / 2),
		thickness,
		static_cast<int>(paddleH)
	};
	SDL_RenderFillRect(mRenderer, &rightPaddle);

	for (int i = 0; i < balls.size(); i++)
	{
		Ball ball = balls[i];
		// Draw ball
		SDL_Rect ballRect{			////�{�[���͓������߁AUpdateGame()�ňʒu��ݒ肵�����ŕ`�悵�Ă���(�ʒu��Vector2�\���̂̕ϐ�mBallPos���g�p�B����̓{�[���̒��S�̈ʒu��\���Ă���)
			static_cast<int>(ball.position.x - thickness / 2),
			static_cast<int>(ball.position.y - thickness / 2),
			thickness,
			thickness
		};
		SDL_RenderFillRect(mRenderer, &ballRect);
	}

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
