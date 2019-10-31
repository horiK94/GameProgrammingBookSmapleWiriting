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
	//InputSystemがKeyboardStateを操作しやすくなる
	friend class InputSystem;
	// Get just the boolean true/false value of key
	//キーの真偽値を返す
	bool GetKeyValue(SDL_Scancode keyCode) const;
	// Get a state based on current and previous frame
	//キーのState(ButtonState)を返す
	ButtonState GetKeyState(SDL_Scancode keyCode) const;
private:
	//現在の状態
	const Uint8* mCurrState;
	//1つ前のフレームの状態(配列 => サイズはSDLがキーボードを管理するバッファサイズに依存)
	Uint8 mPrevState[SDL_NUM_SCANCODES];
};
//
//// Helper for mouse input
class MouseState
{
public:
	friend class InputSystem;

	// For mouse position
	//マウス位置
	const Vector2& GetPosition() const { return mMousePos; }
	//const Vector2& GetScrollWheel() const { return mScrollWheel; }
	const Vector2& GetScrollWheel() const { return mScrollWheel; }
	bool IsRelative() const { return mIsRelative; }

	// For buttons
	//ボタンの状態取得(引数に対応するボタンのビット値を入れる)
	bool GetButtonValue(int button) const;
	ButtonState GetButtonState(int button) const;
private:
	// Store current mouse position
	//マウスの位置
	Vector2 mMousePos;
	// Motion of scroll wheel
	Vector2 mScrollWheel;
	// Store button data
	//現在のマウスボタンの状態
	Uint32 mCurrButtons;
	//前フレームのマウスボタンの状態
	Uint32 mPrevButtons;
	// Are we in relative mouse mode
	//相対モードかどうかの変数
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
	Uint8 mCurrButtons[SDL_CONTROLLER_BUTTON_MAX];		//今回のボタン
	Uint8 mPrevButtons[SDL_CONTROLLER_BUTTON_MAX];		//前回のボタン
	// Left/right sticks
	Vector2 mLeftStick;
	Vector2 mRightStick;
	//// Left/right trigger
	//トリガー
	float mLeftTrigger;
	float mRightTrigger;
	// Is this controller connected?
	bool mIsConnected;		//コントローラーに接続されているか
};

// Wrapper that contains current state of input
//現在の入力情報を格納するラッパー
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
	//SDL_PollEventsループの直前に呼ばれる
	void PrepareForUpdate();
	// Called after SDL_PollEvents loop
	//SDL_PollEventsループの直後に呼ばれる
	void Update();
	// Called to process an SDL event in input system
	//SDLのイベントが呼ばれた時に呼ぶ関数s
	void ProcessEvent(union SDL_Event& event);
	//InputState(入力情報)の参照を返す
	const InputState& GetState() const { return mState; }

	//マウスのポインタを相対モードにするか
	void SetRelativeMouseMode(bool value);
private:
	//トリガーの入力データをフィルターに通し、[-1, 1]の値に変換する関数
	float Filter1D(int input);
	Vector2 Filter2D(int inputX, int inputY);
	InputState mState;
	//コントローラの状態変数
	SDL_GameController* mController;
};
