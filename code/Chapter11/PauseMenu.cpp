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
	//��Βl���[�h�ɂ���(�{�^���̓����蔻�茟���Ɏg�ps)
	SetRelativeMouseMode(false);
	SetTitle("PauseTitle");
	//�{�^���̒ǉ�
	AddButton("ResumeButton", [this]() {
		//UI�\�������(UI�̕\���X�e�[�g��Close�ɂ���)
		Close();
	});
	AddButton("QuitButton", [this]() {
		//�Q�[���̏�ԃX�e�[�g��Quit�ɂ���
		/*mGame->SetState(Game::EQuit);*/
		new DialogBox(mGame, "QuitText", [this]() {
			mGame->SetState(Game::EQuit);
		});
	});
}

PauseMenu::~PauseMenu()
{
	//���΃��[�h�ɂ���(����ŁA�}�E�X�ɂ��J�����̉�]���ł���悤�ɂ��Ƃɖ߂�)
	SetRelativeMouseMode(true);
	//mGame->SetState(Game::EGameplay);
	mGame->SetState(Game::EGameplay);
}

//�L�[�������ꂽ��Ă΂��֐�
void PauseMenu::HandleKeyPress(int key)
{
	UIScreen::HandleKeyPress(key);
	
	if (key == SDLK_ESCAPE)
	{
		Close();
	}
}
