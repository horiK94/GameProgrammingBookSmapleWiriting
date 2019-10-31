// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include <SDL/SDL_scancode.h>
#include <SDL/SDL_gamecontroller.h>
#include <SDL/SDL_mouse.h>
#include "Math.h"

// The different button states
enum ButtonState
{
	ENone,
	EPressed,
	EReleased,
	EHeld
};

 //Helper for keyboard input
class KeyboardState
{
public:
	// Friend so InputSystem can easily update it
	//InputSystem��KeyboardState�𑀍삵�₷���Ȃ�
	friend class InputSystem;
	// Get just the boolean true/false value of key
	//�L�[�̐^�U�l��Ԃ�
	bool GetKeyValue(SDL_Scancode keyCode) const;
	// Get a state based on current and previous frame
	//�L�[��State(ButtonState)��Ԃ�
	ButtonState GetKeyState(SDL_Scancode keyCode) const;
private:
	//���݂̏��
	const Uint8* mCurrState;
	//1�O�̃t���[���̏��(�z�� => �T�C�Y��SDL���L�[�{�[�h���Ǘ�����o�b�t�@�T�C�Y�Ɉˑ�)
	Uint8 mPrevState[SDL_NUM_SCANCODES];
};
//
//// Helper for mouse input
class MouseState
{
public:
	friend class InputSystem;

	// For mouse position
	//�}�E�X�ʒu
	const Vector2& GetPosition() const { return mMousePos; }
	//const Vector2& GetScrollWheel() const { return mScrollWheel; }
	const Vector2& GetScrollWheel() const { return mScrollWheel; }
	bool IsRelative() const { return mIsRelative; }

	// For buttons
	//�{�^���̏�Ԏ擾(�����ɑΉ�����{�^���̃r�b�g�l������)
	bool GetButtonValue(int button) const;
	ButtonState GetButtonState(int button) const;
private:
	// Store current mouse position
	//�}�E�X�̈ʒu
	Vector2 mMousePos;
	// Motion of scroll wheel
	Vector2 mScrollWheel;
	// Store button data
	//���݂̃}�E�X�{�^���̏��
	Uint32 mCurrButtons;
	//�O�t���[���̃}�E�X�{�^���̏��
	Uint32 mPrevButtons;
	// Are we in relative mouse mode
	//���΃��[�h���ǂ����̕ϐ�
	bool mIsRelative;
};

 //Helper for controller input
class ControllerState
{
public:
	friend class InputSystem;

	// For buttons
	bool GetButtonValue(SDL_GameControllerButton button) const;
	ButtonState GetButtonState(SDL_GameControllerButton button) const;

	const Vector2& GetLeftStick() const { return mLeftStick; }
	const Vector2& GetRightStick() const { return mRightStick; }
	float GetLeftTrigger() const { return mLeftTrigger; }
	float GetRightTrigger() const { return mRightTrigger; }

	//bool GetIsConnected() const { return mIsConnected; }
	bool GetIsConnected() const { return mIsConnected; }
private:
	// Current/previous buttons
	Uint8 mCurrButtons[SDL_CONTROLLER_BUTTON_MAX];		//����̃{�^��
	Uint8 mPrevButtons[SDL_CONTROLLER_BUTTON_MAX];		//�O��̃{�^��
	// Left/right sticks
	Vector2 mLeftStick;
	Vector2 mRightStick;
	//// Left/right trigger
	//�g���K�[
	float mLeftTrigger;
	float mRightTrigger;
	// Is this controller connected?
	bool mIsConnected;		//�R���g���[���[�ɐڑ�����Ă��邩
};

// Wrapper that contains current state of input
//���݂̓��͏����i�[���郉�b�p�[
struct InputState
{
	//KeyboardState Keyboard;
	KeyboardState Keyboard;
	//MouseState Mouse;
	MouseState Mouse;
	//ControllerState Controller;
	ControllerState Controller;
};

class InputSystem
{
public:
	bool Initialize();
	void Shutdown();

	// Called right before SDL_PollEvents loop
	//SDL_PollEvents���[�v�̒��O�ɌĂ΂��
	void PrepareForUpdate();
	// Called after SDL_PollEvents loop
	//SDL_PollEvents���[�v�̒���ɌĂ΂��
	void Update();
	// Called to process an SDL event in input system
	//SDL�̃C�x���g���Ă΂ꂽ���ɌĂԊ֐�s
	void ProcessEvent(union SDL_Event& event);
	//InputState(���͏��)�̎Q�Ƃ�Ԃ�
	const InputState& GetState() const { return mState; }

	//�}�E�X�̃|�C���^�𑊑΃��[�h�ɂ��邩
	void SetRelativeMouseMode(bool value);
private:
	//�g���K�[�̓��̓f�[�^���t�B���^�[�ɒʂ��A[-1, 1]�̒l�ɕϊ�����֐�
	float Filter1D(int input);
	Vector2 Filter2D(int inputX, int inputY);
	InputState mState;
	//�R���g���[���̏�ԕϐ�
	SDL_GameController* mController;
};
