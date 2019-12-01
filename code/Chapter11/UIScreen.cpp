// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "UIScreen.h"
#include "Texture.h"
#include "Shader.h"
#include "Game.h"
#include "Renderer.h"
#include "Font.h"

UIScreen::UIScreen(Game* game)
	:mGame(game)
	,mTitle(nullptr)
	,mBackground(nullptr)
	,mTitlePos(0.0f, 300.0f)
	,mNextButtonPos(0.0f, 200.0f)
	,mBGPos(0.0f, 250.0f)
	,mState(EActive)
{
	// Add to UI Stack
	mGame->PushUI(this);
	//�p��ƃ��V�A�ꂵ���Ή����Ă��Ȃ����ꂾ����
	mFont = mGame->GetFont("Assets/Carlito-Regular.ttf");
	mButtonOn = mGame->GetRenderer()->GetTexture("Assets/ButtonYellow.png");
	mButtonOff = mGame->GetRenderer()->GetTexture("Assets/ButtonBlue.png");
}

UIScreen::~UIScreen()
{
	if (mTitle)
	{
		mTitle->Unload();
		delete mTitle;
	}

	for (auto b : mButtons)
	{
		delete b;
	}
	mButtons.clear();
}

void UIScreen::Update(float deltaTime)
{
	
}

void UIScreen::Draw(Shader* shader)
{
	// Draw background (if exists)
	if (mBackground)
	{
		DrawTexture(shader, mBackground, mBGPos);
	}
	// Draw title (if exists)
	if (mTitle)
	{
		DrawTexture(shader, mTitle, mTitlePos);
	}
	// Draw buttons
	//�{�^���̕`��
	for (auto b : mButtons)
	{
		// Draw background of button
		//�{�^�����I������Ă��邩�ŕ\������摜��ύX����
		Texture* tex = b->GetHighlighted() ? mButtonOn : mButtonOff;
		//�{�^���̔w�i��`��
		DrawTexture(shader, tex, b->GetPosition());		//�V�F�[�_�[�ATexture�A�ʒu�����Ƃɕ`��
		// Draw text of button
		//�{�^����UI(���O)�̕`��
		DrawTexture(shader, b->GetNameTex(), b->GetPosition());
	}
	// Override in subclasses to draw any textures
}

void UIScreen::ProcessInput(const uint8_t* keys)
{
	// Do we have buttons?
	if (!mButtons.empty())
	{
		//�{�^�����L��ꍇ
		// Get position of mouse
		int x, y;
		//�}�E�X�̐�Βl���擾
		SDL_GetMouseState(&x, &y);
		// Convert to (0,0) center coordinates
		//��ʂ̐^�񒆂�(0, 0).����ł����ƉE����(512, 384)�̍��W�ɕϊ�����
		Vector2 mousePos(static_cast<float>(x), static_cast<float>(y));
		mousePos.x -= mGame->GetRenderer()->GetScreenWidth() * 0.5f;
		mousePos.y = mGame->GetRenderer()->GetScreenHeight() * 0.5f - mousePos.y;
		
		// Highlight any buttons
		//�S�{�^���ɑ΂��ă{�^���̒��Ƀ}�E�X�����邩���ׂ�
		for (auto b : mButtons)
		{
			if (b->ContainsPoint(mousePos))
			{
				//�}�E�X���{�^����ɂ���Ƃ�
				//Button�̃t���O�𗧂Ă�
				//�t���O�����ƂɃ{�^���̔w�i�̉摜��A�����ꂽ�Ƃ���onClick()���Ă�Button�����߂�
				b->SetHighlighted(true);
			}
			else
			{
				//Button�̃t���O��܂�
				b->SetHighlighted(false);
			}
		}
	}
}

void UIScreen::HandleKeyPress(int key)
{
	switch (key)
	{
	case SDL_BUTTON_LEFT:
		if (!mButtons.empty())
		{
			for (auto b : mButtons)
			{
				if (b->GetHighlighted())
				{
					//�}�E�X���{�^����ɂ���A�Ƃ����t���O�������Ă��āA���N���b�N���ꂽ�Ƃ�����
					//OnClick()���Ă�
					b->OnClick();
					break;
				}
			}
		}
		break;
	default:
		break;
	}
}

void UIScreen::Close()
{
	mState = EClosing;
}

void UIScreen::SetTitle(const std::string& text,
						const Vector3& color,
						int pointSize)
{
	// Clear out previous title texture if it exists
	if (mTitle)
	{
		mTitle->Unload();
		delete mTitle;
		mTitle = nullptr;
	}
	//�����̃e�N�X�`���̍쐬�y�ё��
	mTitle = mFont->RenderText(text, color, pointSize);
}

void UIScreen::AddButton(const std::string& name, std::function<void()> onClick)
{
	//"�{�^�����I������Ă���Ƃ��̃{�^���̔w�i�摜"�̃T�C�Y�����ƂɁA�{�^���̃T�C�Y��ݒ�
	Vector2 dims(static_cast<float>(mButtonOn->GetWidth()), 
		static_cast<float>(mButtonOn->GetHeight()));
	//�ʒu�Ȃǂ̏����w��
	Button* b = new Button(name, mFont, onClick, mNextButtonPos, dims);
	//�{�^���Q�ɒǉ�
	mButtons.emplace_back(b);

	// Update position of next button
	// Move down by height of button plus padding
	//���̃{�^���̈ʒu�������Ă���
	mNextButtonPos.y -= mButtonOff->GetHeight() + 20.0f;
}

void UIScreen::DrawTexture(class Shader* shader, class Texture* texture,
				 const Vector2& offset, float scale)
{
	// Scale the quad by the width/height of texture
	Matrix4 scaleMat = Matrix4::CreateScale(
		static_cast<float>(texture->GetWidth()) * scale,
		static_cast<float>(texture->GetHeight()) * scale,
		1.0f);
	// Translate to position on screen
	Matrix4 transMat = Matrix4::CreateTranslation(
		Vector3(offset.x, offset.y, 0.0f));	
	// Set world transform
	Matrix4 world = scaleMat * transMat;
	shader->SetMatrixUniform("uWorldTransform", world);
	// Set current texture
	texture->SetActive();
	// Draw quad
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

//���΃}�E�X���[�h���g�p���邩�ݒ肷��
void UIScreen::SetRelativeMouseMode(bool relative)
{
	if (relative)
	{
		SDL_SetRelativeMouseMode(SDL_TRUE);
		// Make an initial call to get relative to clear out
		//�}�E�X�̍Ō�ɋL�^���ꂽ���W�������ɑ�������
		//�����nullptr���w�肵�Ă���B�܂�A���̃}�E�X�̈ʒu���L�����Ă����A
		//���̃t���[�����琳�������Βl���g����悤�ɂ��Ă���
		SDL_GetRelativeMouseState(nullptr, nullptr);
	}
	else
	{
		//��Βl�Ŏg�p����
		SDL_SetRelativeMouseMode(SDL_FALSE);
	}
}

Button::Button(const std::string& name, Font* font,
	std::function<void()> onClick,
	const Vector2& pos, const Vector2& dims)
	:mOnClick(onClick)
	,mNameTex(nullptr)
	,mFont(font)
	,mPosition(pos)
	,mDimensions(dims)
	,mHighlighted(false)
{
	SetName(name);
}

Button::~Button()
{
	if (mNameTex)
	{
		mNameTex->Unload();
		delete mNameTex;
	}
}

void Button::SetName(const std::string& name)
{
	mName = name;

	if (mNameTex)
	{
		mNameTex->Unload();
		delete mNameTex;
		mNameTex = nullptr;
	}
	//�����̃e�N�X�`���̍쐬�y�ё��(�{�^���w�i��UIScrren�̃T�|�[�g�֐����Ή�)
	mNameTex = mFont->RenderText(mName);
}

bool Button::ContainsPoint(const Vector2& pt) const
{
	bool no = pt.x < (mPosition.x - mDimensions.x / 2.0f) ||
		pt.x > (mPosition.x + mDimensions.x / 2.0f) ||
		pt.y < (mPosition.y - mDimensions.y / 2.0f) ||
		pt.y > (mPosition.y + mDimensions.y / 2.0f);
	return !no;
}

void Button::OnClick()
{
	// Call attached handler, if it exists
	//�n���h������������Ă�
	if (mOnClick)
	{
		mOnClick();
	}
}
