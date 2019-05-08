#pragma once
#include "SpriteComponent.h"
#include <string>
#include "Math.h"
#include <vector> 

class TileMapComponent :
	public SpriteComponent
{
public:
	TileMapComponent(Actor* actor, int drawOrder = 100);
	~TileMapComponent();
	//�^�C���Z�b�g�̉摜�f�[�^�̓ǂݍ���
	void SetTileSet(SDL_Texture* tileSet, Vector2 screenSize, int tileXNum, int tileYNum);
	void Draw(SDL_Renderer* renderer) override;
	void SetTileData(std::string fileName);
private:
	void GetDrawSetMapRect(SDL_Rect* rect, int id);		//�w���id�̃^�C���}�b�vRect�f�[�^�̎擾
	void DrawTile(SDL_Renderer* renderer, int x, int y, int id);		//�^�C���̕`��

	//�c, ���̕\����
	int tileWidth;
	int tileHeight;

	//�^�C���Z�b�g�T�C�Y
	Vector2 tileSetSize;
	//�^�C���Z�b�g��x, y���W�̌�
	int tileSetXNum;
	int tileSetYNum;
	//�^�C���}�b�v���
	std::vector<std::vector<int>> tileMapData;

	Vector2 screenSize;
};

