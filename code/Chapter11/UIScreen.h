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
	//クリック時のコールバックはコンストラクターで設定
	//設定内容は名称, フォント、コールバック関数、位置と寸法
	//こっちでもSetName()は呼ばれる
	Button(const std::string& name, class Font* font,
		std::function<void()> onClick,
		const Vector2& pos, const Vector2& dims);
	~Button();

	// Set the name of the button
	//表示テキストの設定と、テクスチャの生成
	void SetName(const std::string& name);

	// Getters/setters
	class Texture* GetNameTex() { return mNameTex; }
	const Vector2& GetPosition() const { return mPosition; }
	void SetHighlighted(bool sel) { mHighlighted = sel; }
	bool GetHighlighted() const { return mHighlighted; }

	// Returns true if the point is within the button's bounds
	//引数の座標がこのボタンの範囲内ならtrueを返す0
	bool ContainsPoint(const Vector2& pt) const;
	// Called when button is clicked
	//ボタンが押されたときに呼ばれる
	void OnClick();
private:
	//ボタンクリック時のコールバック
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
	//画面状態更新
	virtual void Update(float deltaTime);
	//画面描画
	virtual void Draw(class Shader* shader);
	virtual void ProcessInput(const uint8_t* keys);
	virtual void HandleKeyPress(int key);
	// Tracks if the UI is active or closing
	//状態がアクティブかクロージングかを管理
	enum UIState
	{
		EActive,
		EClosing
	};
	//状態をクロージングに
	// Set state to closing
	void Close();
	//状態の取得
	// Get state of UI screen
	UIState GetState() const { return mState; }
	// Change the title text
	//タイトルの文字列の変更
	void SetTitle(const std::string& text,
		const Vector3& color = Color::White,
		int pointSize = 40);
	// Add a button to this screen
	//ボタンを画面に追加(追加したボタンを整列させてくれる便利関数)
	void AddButton(const std::string& name, std::function<void()> onClick);
protected:
	// Helper to draw a texture
	//テクスチャを表示するヘルパー関数(ActorではないからTextureComponentを呼べないため)
	void DrawTexture(class Shader* shader, class Texture* texture,
		const Vector2& offset = Vector2::Zero,
		float scale = 1.0f);
	// Sets the mouse mode to relative or not
	void SetRelativeMouseMode(bool relative);
	class Game* mGame;

	class Font* mFont;
	class Texture* mTitle;
	class Texture* mBackground;
	//ボタンが選択されているときの画像
	class Texture* mButtonOn;
	//ボタンが選択されていないときの画像
	class Texture* mButtonOff;

	// Configure positions
	Vector2 mTitlePos;
	//次のボタンの位置
	Vector2 mNextButtonPos;
	Vector2 mBGPos;

	// State
	UIState mState;
	// List of buttons
	//Buttonポインタ群
	std::vector<Button*> mButtons;
};
