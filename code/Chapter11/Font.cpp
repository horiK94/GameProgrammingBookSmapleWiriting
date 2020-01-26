// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Font.h"
#include "Texture.h"
#include <vector>
#include "Game.h"

Font::Font(class Game* game)
	:mGame(game)
{
	
}

Font::~Font()
{
	
}

bool Font::Load(const std::string& fileName)
{
	// We support these font sizes
	//サポートするフォントサイズ
	std::vector<int> fontSizes = {
		8, 9,
		10, 11, 12, 14, 16, 18,
		20, 22, 24, 26, 28,
		30, 32, 34, 36, 38,
		40, 42, 44, 46, 48,
		52, 56,
		60, 64, 68,
		72
	};
	
	//for (auto& size : fontSizes)
	//{
	//	TTF_Font* font = TTF_OpenFont(fileName.c_str(), size);
	//	if (font == nullptr)
	//	{
	//		SDL_Log("Failed to load font %s in size %d", fileName.c_str(), size);
	//		return false;
	//	}
	//	mFontData.emplace(size, font);
	//}
	//return true;
	for (auto& size : fontSizes)
	{
		//TTF_OpenFontは.ttfファイルをロードし、指定したポイントサイズのTTF_Fontデータしか返さないため、サイズごとに何度も呼び出す必要がある
		TTF_Font* font = TTF_OpenFont(fileName.c_str(), size);
		if (font == nullptr)
		{
			SDL_Log("Failed to load font %s in size %d", fileName.c_str(), size);
			return false;
		}
		mFontData.emplace(size, font);
	}
	return true;
}

void Font::Unload()
{
	for (auto& font : mFontData)
	{
		TTF_CloseFont(font.second);
	}
}

//テクスチャ作成はコストが高いので、文字が変わったら呼ぶ
Texture* Font::RenderText(const std::string& textKey,
						  const Vector3& color /*= Color::White*/,
						  int pointSize /*= 24*/)
{
	////表示するテクスチャ
	//Texture* texture = nullptr;
	//
	//// Convert to SDL_Color
	//SDL_Color sdlColor;
	//sdlColor.r = static_cast<Uint8>(color.x * 255);		// 0 〜 1を 0 〜255までのSDL_Colorに変換
	//sdlColor.g = static_cast<Uint8>(color.y * 255);
	//sdlColor.b = static_cast<Uint8>(color.z * 255);
	//sdlColor.a = 255;
	//
	//// Find the font data for this point size
	//auto iter = mFontData.find(pointSize);
	//if (iter != mFontData.end())
	//{
	//	TTF_Font* font = iter->second;
	//	const std::string& actualText = mGame->GetText(textKey);
	//	// Draw this to a surface (blended for alpha)
	//	SDL_Surface* surf = TTF_RenderUTF8_Blended(font, actualText.c_str(), sdlColor);
	//	if (surf != nullptr)
	//	{
	//		// Convert from surface to texture
	//		texture = new Texture();
	//		texture->CreateFromSurface(surf);
	//		SDL_FreeSurface(surf);
	//	}
	//}
	//else
	//{
	//	SDL_Log("Point size %d is unsupported", pointSize);
	//}
	//
	//return texture;

	//表示するテクスチャ
	Texture* texture = nullptr;

	SDL_Color sdlColor;
	sdlColor.r = static_cast<Uint8>(color.x * 255);		//0 〜 1を 0 〜255までのSDL_Colorに変換, Uint8 -> unsignedchar => 0-255
	sdlColor.g = static_cast<Uint8>(color.y * 255);
	sdlColor.b = static_cast<Uint8>(color.z * 255);
	sdlColor.a = 255;

	auto iter = mFontData.find(pointSize);
	if (iter != mFontData.end())
	{
		//フォントデータが存在する
		TTF_Font* font = iter->second;
		//ロード済みのtextか調べる
		//const std::string& actualText = textKey;
		//JSONで設定したテキストキーに対応するテキストマップの値を受け取る
		const std::string& actualText = mGame->GetText(textKey);
		//TTF_Font*と文字列、色を渡すことで描画を行う
		//グリフの輪郭に設定されている不透明度に応じてアルファブレンディングを行う描画
		//以下の関数だとASCⅡ文字列を受け取るので、日本語が表示できない
		//SDL_Surface* surface = TTF_RenderText_Blended(font, actualText.c_str(), sdlColor);
		//UTF-8でエンコードした文字列を描画したものを取得できる
		SDL_Surface* surface = TTF_RenderUTF8_Blended(font, actualText.c_str(), sdlColor);
		//OpenGLがSDL_Surfaceを描画できないため、Textureに変換する
		if (surface != nullptr)
		{
			texture = new Texture();
			texture->CreateFromSurface(surface);
			SDL_FreeSurface(surface);		//surfaceの解放	
		}
		else
		{
			SDL_Log("Point size %d is unsupported", pointSize);
		}
	}
	return texture;
}
