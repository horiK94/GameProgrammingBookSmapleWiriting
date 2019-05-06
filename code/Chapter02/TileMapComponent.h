#pragma once
#include "SpriteComponent.h"
#include <string>
#include "Math.h"

class TileMapComponent :
	public SpriteComponent
{
public:
	TileMapComponent(Actor* actor);
	~TileMapComponent();
	//�^�C���Z�b�g�̉摜�f�[�^�̓ǂݍ���
	void SetTileSet(SDL_Texture* tileSet, int tileSetXNum, int tileSetYNum);
	//SpriteComponent��Draw�֐��̃I�[�o�[���[�h
	void Draw(SDL_Renderer* renderer, int id = -1);
private:
	SDL_Rect* GetDrawRect(int id);
	//�^�C���Z�b�g�T�C�Y
	Vector2 tileSize;
	//�^�C���Z�b�g��x, y���W�̌�
	int tileSetXNum;
	int tileSetYNum;
	//�^�C���Z�b�g�f�[�^
	SDL_Texture* tileSetTexture;
};

