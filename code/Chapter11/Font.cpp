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
	//�T�|�[�g����t�H���g�T�C�Y
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
		//TTF_OpenFont��.ttf�t�@�C�������[�h���A�w�肵���|�C���g�T�C�Y��TTF_Font�f�[�^�����Ԃ��Ȃ����߁A�T�C�Y���Ƃɉ��x���Ăяo���K�v������
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

//�e�N�X�`���쐬�̓R�X�g�������̂ŁA�������ς������Ă�
Texture* Font::RenderText(const std::string& textKey,
						  const Vector3& color /*= Color::White*/,
						  int pointSize /*= 24*/)
{
	////�\������e�N�X�`��
	//Texture* texture = nullptr;
	//
	//// Convert to SDL_Color
	//SDL_Color sdlColor;
	//sdlColor.r = static_cast<Uint8>(color.x * 255);		// 0 �` 1�� 0 �`255�܂ł�SDL_Color�ɕϊ�
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

	//�\������e�N�X�`��
	Texture* texture = nullptr;

	SDL_Color sdlColor;
	sdlColor.r = static_cast<Uint8>(color.x * 255);		//0 �` 1�� 0 �`255�܂ł�SDL_Color�ɕϊ�, Uint8 -> unsignedchar => 0-255
	sdlColor.g = static_cast<Uint8>(color.y * 255);
	sdlColor.b = static_cast<Uint8>(color.z * 255);
	sdlColor.a = 255;

	auto iter = mFontData.find(pointSize);
	if (iter != mFontData.end())
	{
		//�t�H���g�f�[�^�����݂���
		TTF_Font* font = iter->second;
		//���[�h�ς݂�text�����ׂ�
		//const std::string& actualText = textKey;
		//JSON�Őݒ肵���e�L�X�g�L�[�ɑΉ�����e�L�X�g�}�b�v�̒l���󂯎��
		const std::string& actualText = mGame->GetText(textKey);
		//TTF_Font*�ƕ�����A�F��n�����Ƃŕ`����s��
		//�O���t�̗֊s�ɐݒ肳��Ă���s�����x�ɉ����ăA���t�@�u�����f�B���O���s���`��
		//�ȉ��̊֐�����ASC�U��������󂯎��̂ŁA���{�ꂪ�\���ł��Ȃ�
		//SDL_Surface* surface = TTF_RenderText_Blended(font, actualText.c_str(), sdlColor);
		//UTF-8�ŃG���R�[�h�����������`�悵�����̂��擾�ł���
		SDL_Surface* surface = TTF_RenderUTF8_Blended(font, actualText.c_str(), sdlColor);
		//OpenGL��SDL_Surface��`��ł��Ȃ����߁ATexture�ɕϊ�����
		if (surface != nullptr)
		{
			texture = new Texture();
			texture->CreateFromSurface(surface);
			SDL_FreeSurface(surface);		//surface�̉��	
		}
		else
		{
			SDL_Log("Point size %d is unsupported", pointSize);
		}
	}
	return texture;
}
