// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "Math.h"
#include <cstdint>
#include <string>
#include <functional>
#include <vector>

class Button
{
public:
	//�N���b�N���̃R�[���o�b�N�̓R���X�g���N�^�[�Őݒ�
	//�ݒ���e�͖���, �t�H���g�A�R�[���o�b�N�֐��A�ʒu�Ɛ��@
	//�������ł�SetName()�͌Ă΂��
	Button(const std::string& name, class Font* font,
		std::function<void()> onClick,
		const Vector2& pos, const Vector2& dims);
	~Button();

	// Set the name of the button
	//�\���e�L�X�g�̐ݒ�ƁA�e�N�X�`���̐���
	void SetName(const std::string& name);

	// Getters/setters
	class Texture* GetNameTex() { return mNameTex; }
	const Vector2& GetPosition() const { return mPosition; }
	void SetHighlighted(bool sel) { mHighlighted = sel; }
	bool GetHighlighted() const { return mHighlighted; }

	// Returns true if the point is within the button's bounds
	//�����̍��W�����̃{�^���͈͓̔��Ȃ�true��Ԃ�0
	bool ContainsPoint(const Vector2& pt) const;
	// Called when button is clicked
	//�{�^���������ꂽ�Ƃ��ɌĂ΂��
	void OnClick();
private:
	//�{�^���N���b�N���̃R�[���o�b�N
	std::function<void()> mOnClick;
	std::string mName;
	class Texture* mNameTex;
	class Font* mFont;
	Vector2 mPosition;
	Vector2 mDimensions;
	bool mHighlighted;
};

class UIScreen
{
public:
	UIScreen(class Game* game);
	virtual ~UIScreen();
	// UIScreen subclasses can override these
	//��ʏ�ԍX�V
	virtual void Update(float deltaTime);
	//��ʕ`��
	virtual void Draw(class Shader* shader);
	virtual void ProcessInput(const uint8_t* keys);
	virtual void HandleKeyPress(int key);
	// Tracks if the UI is active or closing
	//��Ԃ��A�N�e�B�u���N���[�W���O�����Ǘ�
	enum UIState
	{
		EActive,
		EClosing
	};
	//��Ԃ��N���[�W���O��
	// Set state to closing
	void Close();
	//��Ԃ̎擾
	// Get state of UI screen
	UIState GetState() const { return mState; }
	// Change the title text
	//�^�C�g���̕�����̕ύX
	void SetTitle(const std::string& text,
		const Vector3& color = Color::White,
		int pointSize = 40);
	// Add a button to this screen
	//�{�^������ʂɒǉ�(�ǉ������{�^���𐮗񂳂��Ă����֗��֐�)
	void AddButton(const std::string& name, std::function<void()> onClick);
protected:
	// Helper to draw a texture
	//�e�N�X�`����\������w���p�[�֐�(Actor�ł͂Ȃ�����TextureComponent���ĂׂȂ�����)
	void DrawTexture(class Shader* shader, class Texture* texture,
		const Vector2& offset = Vector2::Zero,
		float scale = 1.0f);
	// Sets the mouse mode to relative or not
	void SetRelativeMouseMode(bool relative);
	class Game* mGame;

	class Font* mFont;
	class Texture* mTitle;
	class Texture* mBackground;
	//�{�^�����I������Ă���Ƃ��̉摜
	class Texture* mButtonOn;
	//�{�^�����I������Ă��Ȃ��Ƃ��̉摜
	class Texture* mButtonOff;

	// Configure positions
	Vector2 mTitlePos;
	//���̃{�^���̈ʒu
	Vector2 mNextButtonPos;
	Vector2 mBGPos;

	// State
	UIState mState;
	// List of buttons
	//Button�|�C���^�Q
	std::vector<Button*> mButtons;
};
