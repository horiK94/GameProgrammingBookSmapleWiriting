// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "InputSystem.h"
#include <SDL/SDL.h>
#include <cstring>

bool KeyboardState::GetKeyValue(SDL_Scancode keyCode) const
{
	//return mCurrState[keyCode] == 1;
	//1�Ȃ牟����Ă���̂�true��Ԃ�
	return mCurrState[keyCode] == 1;
}

ButtonState KeyboardState::GetKeyState(SDL_Scancode keyCode) const
{
	//if (mPrevState[keyCode] == 0)
	//{
	//	if (mCurrState[keyCode] == 0)
	//	{
	//		return ENone;
	//	}
	//	else
	//	{
	//		return EPressed;
	//	}
	//}
	//else // Prev state must be 1
	//{
	//	if (mCurrState[keyCode] == 0)
	//	{
	//		return EReleased;
	//	}
	//	else
	//	{
	//		return EHeld;
	//	}
	//}
	if (mPrevState[keyCode] == 0)
	{
		if (mCurrState[keyCode] == 0)
		{
			return ButtonState::ENone;
		}
		else
		{
			return ButtonState::EPressed;
		}
	}
	else
	{
		if (mCurrState[keyCode] == 0)
		{
			return ButtonState::EReleased;
		}
		else
		{
			return ButtonState::EHeld;
		}
	}
}

bool MouseState::GetButtonValue(int button) const
{
	//return (SDL_BUTTON(button) & mCurrButtons) == 1;
	return (SDL_BUTTON(button) & mCurrButtons) == 1;
}

ButtonState MouseState::GetButtonState(int button) const
{
	int mask = SDL_BUTTON(button);
	if ((mask & mPrevButtons) == 0)
	{
		if ((mask & mCurrButtons) == 0)
		{
			return ENone;
		}
		else
		{
			return EPressed;
		}
	}
	else
	{
		if ((mask & mCurrButtons) == 0)
		{
			return EReleased;
		}
		else
		{
			return EHeld;
		}
	}
}

bool ControllerState::GetButtonValue(SDL_GameControllerButton button) const
{
	return mCurrButtons[button] == 1;
}

ButtonState ControllerState::GetButtonState(SDL_GameControllerButton button) const
{
	if (mPrevButtons[button] == 0)
	{
		if (mCurrButtons[button] == 0)
		{
			return ENone;
		}
		else
		{
			return EPressed;
		}
	}
	else // Prev state must be 1
	{
		if (mCurrButtons[button] == 0)
		{
			return EReleased;
		}
		else
		{
			return EHeld;
		}
	}
}

bool InputSystem::Initialize()
{
	//// Keyboard
	//// Assign current state pointer
	//mState.Keyboard.mCurrState = SDL_GetKeyboardState(NULL);
	//// Clear previous state memory
	//memset(mState.Keyboard.mPrevState, 0,
	//	SDL_NUM_SCANCODES);

	//// Mouse (just set everything to 0)
	//mState.Mouse.mCurrButtons = 0;
	//mState.Mouse.mPrevButtons = 0;

	//// Get the connected controller, if it exists
	//mController = SDL_GameControllerOpen(0);
	//// Initialize controller state
	//mState.Controller.mIsConnected = (mController != nullptr);
	//memset(mState.Controller.mCurrButtons, 0,
	//	SDL_CONTROLLER_BUTTON_MAX);
	//memset(mState.Controller.mPrevButtons, 0,
	//	SDL_CONTROLLER_BUTTON_MAX);

	//return true;

	//�L�[�{�[�h
	//���݂̃L�[�{�[�h�̏�ԃ|�C���^���擾
	mState.Keyboard.mCurrState = SDL_GetKeyboardState(NULL);
	//�O�̃L�[�{�[�h�̏�ԃ|�C���^��0�ŃN���A
	memset(mState.Keyboard.mPrevState, 0, SDL_NUM_SCANCODES);

	//�}�E�X
	//�A�C�R���̔�\��(�f�t�H���g�͕\��)
	SDL_ShowCursor(SDL_FALSE);
	mState.Mouse.mIsRelative = false;

	//�R���g���[���[
	//�R���g���[���[0���I�[�v��
	mController = SDL_GameControllerOpen(0);
	mState.Controller.mIsConnected = mController != nullptr;
	memset(mState.Controller.mCurrButtons, 0, SDL_CONTROLLER_BUTTON_MAX);		//�{�^���̐^�U�l��false�ŏ�����
	memset(mState.Controller.mPrevButtons, 0, SDL_CONTROLLER_BUTTON_MAX);

	return true;
}

void InputSystem::Shutdown()
{
}

void InputSystem::PrepareForUpdate()
{
	//// Copy current state to previous
	//// Keyboard
	//memcpy(mState.Keyboard.mPrevState,
	//	mState.Keyboard.mCurrState,
	//	SDL_NUM_SCANCODES);

	//// Mouse
	//mState.Mouse.mPrevButtons = mState.Mouse.mCurrButtons;
	//mState.Mouse.mIsRelative = false;
	//mState.Mouse.mScrollWheel = Vector2::Zero;

	//// Controller
	//memcpy(mState.Controller.mPrevButtons,
	//	mState.Controller.mCurrButtons,
	//	SDL_CONTROLLER_BUTTON_MAX);

	//�L�[�{�[�h
	//�O�̃L�[�{�[�h��Ԃ̕ϐ��Ɍ��݂̃L�[�{�[�h��Ԃ̕ϐ�(�Ăяo���^�C�~���O���ɂ͑S�t���[���̃L�[�{�[�h�̏�Ԃ���)���R�s�[����
	memcpy(mState.Keyboard.mPrevState,
		mState.Keyboard.mCurrState,
		SDL_NUM_SCANCODES);

	//�}�E�X
	mState.Mouse.mPrevButtons = mState.Mouse.mCurrButtons;
	//�}�E�X�z�C�[���̕ϐ��̒l������������(���������Ȃ��ƁA�z�C�[���𓮂����Ă��Ȃ��Ă��A�O�̕ϐ��̒l���g�p����Ă��܂�����)
	mState.Mouse.mScrollWheel = Vector2::Zero;

	//�R���g���[��
	//���݂���1�O�ւƃ{�^���̏�Ԃ��R�s�[
	memcpy(mState.Controller.mPrevButtons, mState.Controller.mCurrButtons, SDL_CONTROLLER_BUTTON_MAX);
}

void InputSystem::Update()
{
	//// Mouse
	//int x = 0, y = 0;
	//if (mState.Mouse.mIsRelative)
	//{
	//	mState.Mouse.mCurrButtons = 
	//		SDL_GetRelativeMouseState(&x, &y);
	//}
	//else
	//{
	//	mState.Mouse.mCurrButtons = 
	//		SDL_GetMouseState(&x, &y);
	//}

	//mState.Mouse.mMousePos.x = static_cast<float>(x);
	//mState.Mouse.mMousePos.y = static_cast<float>(y);

	//// Controller
	//// Buttons
	//for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; i++)
	//{
	//	mState.Controller.mCurrButtons[i] =
	//		SDL_GameControllerGetButton(mController, 
	//			SDL_GameControllerButton(i));
	//}

	//// Triggers
	//mState.Controller.mLeftTrigger =
	//	Filter1D(SDL_GameControllerGetAxis(mController,
	//		SDL_CONTROLLER_AXIS_TRIGGERLEFT));
	//mState.Controller.mRightTrigger =
	//	Filter1D(SDL_GameControllerGetAxis(mController,
	//		SDL_CONTROLLER_AXIS_TRIGGERRIGHT));

	//// Sticks
	//x = SDL_GameControllerGetAxis(mController,
	//	SDL_CONTROLLER_AXIS_LEFTX);
	//y = -SDL_GameControllerGetAxis(mController,
	//	SDL_CONTROLLER_AXIS_LEFTY);
	//mState.Controller.mLeftStick = Filter2D(x, y);

	//x = SDL_GameControllerGetAxis(mController,
	//	SDL_CONTROLLER_AXIS_RIGHTX);
	//y = -SDL_GameControllerGetAxis(mController,
	//	SDL_CONTROLLER_AXIS_RIGHTY);
	//mState.Controller.mRightStick = Filter2D(x, y);
	
	//�}�E�X
	int x = 0, y = 0;
	if (mState.Mouse.IsRelative())
	{
		mState.Mouse.mCurrButtons = SDL_GetRelativeMouseState(&x, &y);
	}
	else
	{
		mState.Mouse.mCurrButtons = SDL_GetMouseState(&x, &y);		//�ʒu�y�у{�^���̏�Ԃ̎擾(������Ă���̂̓{�^���̏��)
	}
	mState.Mouse.mMousePos.x = static_cast<float>(x);		//float�ɕϊ�
	mState.Mouse.mMousePos.y = static_cast<float>(y);

	//�R���g���[��
	//�{�^��
	//�e�{�^���̏�Ԃ�₢���킹��
	for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; i++)
	{
		mState.Controller.mCurrButtons[i] = SDL_GameControllerGetButton(mController, SDL_GameControllerButton(i));
	}

	//�g���K�[
	mState.Controller.mLeftTrigger = Filter1D(SDL_GameControllerGetAxis(mController, SDL_CONTROLLER_AXIS_TRIGGERLEFT));
	mState.Controller.mRightTrigger = Filter1D(SDL_GameControllerGetAxis(mController, SDL_CONTROLLER_AXIS_TRIGGERRIGHT));

	//�X�e�B�b�N
	x = SDL_GameControllerGetAxis(mController, SDL_CONTROLLER_AXIS_LEFTX);
	y = -SDL_GameControllerGetAxis(mController, SDL_CONTROLLER_AXIS_LEFTY);		//SDL�ł�y����������+�ŕԂ�����
	mState.Controller.mLeftStick = Filter2D(x, y);

	x = SDL_GameControllerGetAxis(mController, SDL_CONTROLLER_AXIS_RIGHTX);
	y = -SDL_GameControllerGetAxis(mController, SDL_CONTROLLER_AXIS_RIGHTY);		//SDL�ł�y����������+�ŕԂ�����
	mState.Controller.mRightStick = Filter2D(x, y);
}

void InputSystem::ProcessEvent(SDL_Event& event)
{
	//switch (event.type)
	//{
	//case SDL_MOUSEWHEEL:
	//	mState.Mouse.mScrollWheel = Vector2(
	//		static_cast<float>(event.wheel.x),
	//		static_cast<float>(event.wheel.y));
	//	break;
	//default:
	//	break;
	//}
	switch (event.type)
	{
	case SDL_MOUSEWHEEL:
		mState.Mouse.mScrollWheel = Vector2(
			static_cast<float>(event.wheel.x),
			static_cast<float>(event.wheel.y)
		);
		break;
	default:
		break;
	}
}

void InputSystem::SetRelativeMouseMode(bool value)
{
	//SDL_bool set = value ? SDL_TRUE : SDL_FALSE;
	//SDL_SetRelativeMouseMode(set);

	//mState.Mouse.mIsRelative = value;

	SDL_bool set = value ? SDL_TRUE : SDL_FALSE;
	SDL_SetRelativeMouseMode(set);

	mState.Mouse.mIsRelative = value;
}

float InputSystem::Filter1D(int input)
{
	// A value < dead zone is interpreted as 0%
	//�f�b�h�]�[��
	const int deadZone = 250;
	// A value > max value is interpreted as 100%
	//�ő�l
	const int maxValue = 30000;

	float retVal = 0.0f;

	// Take absolute value of input
	//��Βl�̎擾
	int absValue = input > 0 ? input : -input;
	// Ignore input within dead zone
	//�f�b�h�]�[���ȉ��Ȃ�0�ŏI��
	if (absValue > deadZone)
	{
		// Compute fractional value between dead zone and max value
		retVal = static_cast<float>(absValue - deadZone) /
		(maxValue - deadZone);
		// Make sure sign matches original value
		retVal = input > 0 ? retVal : -1.0f * retVal;
		// Clamp between -1.0f and 1.0f
		//input��maxValue�𒴂���l�̂Ƃ��ɁAretVal = 1.0�ɂȂ�悤����
		retVal = Math::Clamp(retVal, -1.0f, 1.0f);
	}

	return retVal;
}

Vector2 InputSystem::Filter2D(int inputX, int inputY)
{
	const float deadZone = 8000.0f;
	const float maxValue = 30000.0f;

	// Make into 2D vector
	//���͒l��Vector2��
	Vector2 dir;
	dir.x = static_cast<float>(inputX);
	dir.y = static_cast<float>(inputY);

	float length = dir.Length();

	// If length < deadZone, should be no input
	if (length < deadZone)
	{
		dir = Vector2::Zero;
	}
	else
	{
		// Calculate fractional value between
		// dead zone and max value circles
		float f = (length - deadZone) / (maxValue - deadZone);
		// Clamp f between 0.0f and 1.0f
		f = Math::Clamp(f, 0.0f, 1.0f);
		// Normalize the vector, and then scale it to the
		// fractional value
		dir *= f / length;
	}

	return dir;
}
