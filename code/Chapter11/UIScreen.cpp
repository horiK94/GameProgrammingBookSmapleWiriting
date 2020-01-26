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
	//英語とロシア語しか対応していない言語だった
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
	//ボタンの描画
	for (auto b : mButtons)
	{
		// Draw background of button
		//ボタンが選択されているかで表示する画像を変更する
		Texture* tex = b->GetHighlighted() ? mButtonOn : mButtonOff;
		//ボタンの背景を描画
		DrawTexture(shader, tex, b->GetPosition());		//シェーダー、Texture、位置をもとに描画
		// Draw text of button
		//ボタンのUI(名前)の描画
		DrawTexture(shader, b->GetNameTex(), b->GetPosition());
	}
	// Override in subclasses to draw any textures
}

void UIScreen::ProcessInput(const uint8_t* keys)
{
	// Do we have buttons?
	if (!mButtons.empty())
	{
		//ボタンが有る場合
		// Get position of mouse
		int x, y;
		//マウスの絶対値を取得
		SDL_GetMouseState(&x, &y);
		// Convert to (0,0) center coordinates
		//画面の真ん中を(0, 0).今回でいうと右下が(512, 384)の座標に変換する
		Vector2 mousePos(static_cast<float>(x), static_cast<float>(y));
		mousePos.x -= mGame->GetRenderer()->GetScreenWidth() * 0.5f;
		mousePos.y = mGame->GetRenderer()->GetScreenHeight() * 0.5f - mousePos.y;
		
		// Highlight any buttons
		//全ボタンに対してボタンの中にマウスがあるか調べる
		for (auto b : mButtons)
		{
			if (b->ContainsPoint(mousePos))
			{
				//マウスがボタン上にあるとき
				//Buttonのフラグを立てる
				//フラグをもとにボタンの背景の画像や、押されたときにonClick()を呼ぶButtonを決める
				b->SetHighlighted(true);
			}
			else
			{
				//Buttonのフラグを折る
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
					//マウスがボタン上にある、というフラグが立っていて、左クリックされたという状況
					//OnClick()を呼ぶ
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
	//文字のテクスチャの作成及び代入
	mTitle = mFont->RenderText(text, color, pointSize);
}

void UIScreen::AddButton(const std::string& name, std::function<void()> onClick)
{
	//"ボタンが選択されているときのボタンの背景画像"のサイズをもとに、ボタンのサイズを設定
	Vector2 dims(static_cast<float>(mButtonOn->GetWidth()), 
		static_cast<float>(mButtonOn->GetHeight()));
	//位置などの情報を指定
	Button* b = new Button(name, mFont, onClick, mNextButtonPos, dims);
	//ボタン群に追加
	mButtons.emplace_back(b);

	// Update position of next button
	// Move down by height of button plus padding
	//次のボタンの位置を代入しておく
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

//相対マウスモードを使用するか設定する
void UIScreen::SetRelativeMouseMode(bool relative)
{
	if (relative)
	{
		SDL_SetRelativeMouseMode(SDL_TRUE);
		// Make an initial call to get relative to clear out
		//マウスの最後に記録された座標が引数に代入される
		//今回はnullptrを指定している。つまり、今のマウスの位置を記憶しておき、
		//次のフレームから正しい相対値が使えるようにしている
		SDL_GetRelativeMouseState(nullptr, nullptr);
	}
	else
	{
		//絶対値で使用する
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
	//文字のテクスチャの作成及び代入(ボタン背景はUIScrrenのサポート関数が対応)
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
	//ハンドラがあったら呼ぶ
	if (mOnClick)
	{
		mOnClick();
	}
}
