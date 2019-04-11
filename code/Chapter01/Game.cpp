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
	int sdlResult = SDL_Init(SDL_INIT_VIDEO);		//SDLの初期化
	if (sdlResult != 0)
	{
		//初期化失敗
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());		//SDL_Logはc#でいうCOnsole.Log()的なやつ
		return false;
	}
	
	// Create an SDL Window
	mWindow = SDL_CreateWindow(
		"Game Programming in C++ (Chapter 1)", // Window title		(日本語はバグるので注意)
		100,	// Top left x-coordinate of window
		100,	// Top left y-coordinate of window
		1024,	// Width of window
		768,	// Height of window
		0		// Flags (0 for no flags set)
	);

	if (!mWindow)
	{

		// windowの作成失敗
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}
	
	//// Create SDL renderer
	mRenderer = SDL_CreateRenderer(
		mWindow, // Window to create renderer for(作成するレンダラーの描画対象となるウィンドウ)
		-1,		 // Usually -1（グラフィックドライバー. ウィンドウ1つなのでデフォルトの-1を選ぶことでSDLが決めてくれる）
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);
	//SDL_RENDERER_ACCELERATED..グラフィックスハードウェアの機能を仕様
	//SDL_RENDERER_PRESENTVSYNC..垂直同期を用いる

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
		ProcessInput();		//入力受付
		UpdateGame();		//更新
		GenerateOutput();	//描画
	}
}

void Game::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))		//イベントがあればtrueに
	{
		//複数のイベントが入っている可能性があるので、すべてのイベントループ処理する必要がある
		switch (event.type)
		{
			// If we get an SDL_QUIT event, end loop
			case SDL_QUIT:
				mIsRunning = false;
				break;
		}
	}

	// Get state of keyboard
	const Uint8* state = SDL_GetKeyboardState(NULL);		//キーボード全体の状態が入った配列へのポインタを返す
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

	//16ms過ぎていたら以下を実行
	// Delta time is the difference in ticks from last frame
	// (converted to seconds)
	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	
	// Clamp maximum delta time value
	//ブレイクポイントとかで50ms超えていたら、50msとして扱う
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}

	//次フレームのため、現在の経過時間を保存
	// Update tick counts (for next frame)
	mTicksCount = SDL_GetTicks();
	
	//バドルを操作時
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
	//バックバッファを単色で塗りつぶす(前に描画した内容の上に描画しないため)
	//ここでレンダラーへのポインタを受け取る
	// Set draw color to blue
	SDL_SetRenderDrawColor(
		mRenderer,
		0,		// R
		0,		// G 
		255,	// B
		255		// A
	);
	
	// Clear back buffer
	SDL_RenderClear(mRenderer);		//現在のレンダーターゲットを色で塗りつぶし(最後のSDL_RenderPreset()を呼ばないとフロントバッファとバックバッファが交換されず真っ白な画面になってしまう)



	//描画はバックバッファをクリアしたあと、フロントバッファと交換するまでに行う

	// Draw walls
	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);		//mRendereに白の参照を渡す
	
	// Draw top wall
	SDL_Rect wall{		//SDL_Rectの引数はすべてint型で指定されている
		0,			// Top left x
		0,			// Top left y
		1024,		// Width
		thickness	// Height
	};
	SDL_RenderFillRect(mRenderer, &wall);		//塗りつぶされた長方形の描画

	// Draw bottom wall
	wall.y = 768 - thickness;		//高さは768で指定しているため
	SDL_RenderFillRect(mRenderer, &wall);
	
	// Draw right wall
	wall.x = 1024 - thickness;
	wall.y = 0;
	wall.w = thickness;
	wall.h = 1024;
	SDL_RenderFillRect(mRenderer, &wall);

	// Draw paddle(パドルの描画)
	SDL_Rect paddle{		//パドルは動かすため、UpdateGame()で位置を設定しここで描画している(位置はVector2構造体の変数mPaddlePosを使用。これはパドルの左真ん中の位置を表している)
		static_cast<int>(mPaddlePos.x),		//int型への変更.SDL_Rectの引数はすべてint型のため.static_castの暗黙の型変換を行う
		static_cast<int>(mPaddlePos.y - paddleH/2),
		thickness,
		static_cast<int>(paddleH)
	};
	SDL_RenderFillRect(mRenderer, &paddle);
	
	// Draw ball
	SDL_Rect ball{			////ボールは動くため、UpdateGame()で位置を設定しここで描画している(位置はVector2構造体の変数mBallPosを使用。これはボールの中心の位置を表している)
		static_cast<int>(mBallPos.x - thickness/2),
		static_cast<int>(mBallPos.y - thickness/2),
		thickness,
		thickness
	};
	SDL_RenderFillRect(mRenderer, &ball);
	
	// Swap front buffer and back buffer
	//これ呼ばないと真っ白なまま
	SDL_RenderPresent(mRenderer);
}

void Game::Shutdown()
{
	SDL_DestroyRenderer(mRenderer);		//レンダラーの終了
	SDL_DestroyWindow(mWindow);		//ウィンドウの終了
	SDL_Quit();		//SDLの終了
}
