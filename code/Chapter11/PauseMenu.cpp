// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "PauseMenu.h"
#include "Game.h"
#include "DialogBox.h"
#include <SDL/SDL.h>

PauseMenu::PauseMenu(Game* game)
	:UIScreen(game)
{
	//mGame->SetState(Game::EPaused);
	//SetRelativeMouseMode(false);
	//SetTitle("PauseTitle");
	//AddButton("ResumeButton", [this]() {
	//	Close();
	//});
	//AddButton("QuitButton", [this]() { 
	//	new DialogBox(mGame, "QuitText",
	//		[this]() {
	//			mGame->SetState(Game::EQuit);
	//	});
	//});
	mGame->SetState(Game::EPaused);
	//絶対値モードにする(ボタンの当たり判定検査に使用s)
	SetRelativeMouseMode(false);
	SetTitle("PauseTitle");
	//ボタンの追加
	AddButton("ResumeButton", [this]() {
		//UI表示を閉じる(UIの表示ステートをCloseにする)
		Close();
	});
	AddButton("QuitButton", [this]() {
		//ゲームの状態ステートをQuitにする
		/*mGame->SetState(Game::EQuit);*/
		new DialogBox(mGame, "QuitText", [this]() {
			mGame->SetState(Game::EQuit);
		});
	});
}

PauseMenu::~PauseMenu()
{
	//相対モードにする(これで、マウスによるカメラの回転ができるようにもとに戻る)
	SetRelativeMouseMode(true);
	//mGame->SetState(Game::EGameplay);
	mGame->SetState(Game::EGameplay);
}

//キーが押されたら呼ばれる関数
void PauseMenu::HandleKeyPress(int key)
{
	UIScreen::HandleKeyPress(key);
	
	if (key == SDLK_ESCAPE)
	{
		Close();
	}
}
