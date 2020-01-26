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
	//初期化
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
	//右パドルの入力処理
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

	//右パドルの操作
	if (rightPaddleDir != 0)
	{
		//0以外のとき = 移動したいとき
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

	//ballの更新
	for (int i = 0; i < balls.size(); i++)
	{
		Ball ball = balls[i];
		ball.position.x += ball.motionVector.x * deltaTime;
		ball.position.y += ball.motionVector.y * deltaTime;

		//パドルとボールの位置差(位置関係を調べる)
		float diffLeft = leftPaddlePos.y - ball.position.y;
		//差の絶対値を求める
		diffLeft = (diffLeft > 0.0f) ? diffLeft : -diffLeft;
		if (
			//差がパドルの大きさの中に入っている
			diffLeft <= paddleH / 2.0f &&
			// ボールが特定のxのいちにあるか
			ball.position.x <= 25.0f && ball.position.x >= 20.0f &&
			// ボールのベクトルがパドルに向かう方向か
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
		//どちらかのチームの負けのためゲーム終了
		if (ball.position.x <= 0.0f || ball.position.x >= 1024)
		{
			mIsRunning = false;
		}

		//上の壁との衝突
		if (ball.position.y <= thickness && ball.position.y < 0.0f)
		{
			ball.motionVector.y *= -1;
		}
		//下の壁との衝突
		else if (ball.position.y >= (768 - thickness) &&
			ball.motionVector.y > 0.0f)
		{
			ball.motionVector.y *= -1;
		}

		// Ballは構造体のため、最後に結果を入れ直す
		balls[i] = ball;
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
	/*
	wall.x = 1024 - thickness;
	wall.y = 0;
	wall.w = thickness;
	wall.h = 1024;
	SDL_RenderFillRect(mRenderer, &wall);
	*/

	// Draw paddle(左パドルの描画)
	SDL_Rect leftPaddle{		//パドルは動かすため、UpdateGame()で位置を設定しここで描画している(位置はVector2構造体の変数leftPaddlePosを使用。これはパドルの左真ん中の位置を表している)
		static_cast<int>(leftPaddlePos.x),		//int型への変更.SDL_Rectの引数はすべてint型のため.static_castの暗黙の型変換を行う
		static_cast<int>(leftPaddlePos.y - paddleH / 2),
		thickness,
		static_cast<int>(paddleH)
	};
	SDL_RenderFillRect(mRenderer, &leftPaddle);

	// 右パドルの描画
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
		SDL_Rect ballRect{			////ボールは動くため、UpdateGame()で位置を設定しここで描画している(位置はVector2構造体の変数mBallPosを使用。これはボールの中心の位置を表している)
			static_cast<int>(ball.position.x - thickness / 2),
			static_cast<int>(ball.position.y - thickness / 2),
			thickness,
			thickness
		};
		SDL_RenderFillRect(mRenderer, &ballRect);
	}

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
