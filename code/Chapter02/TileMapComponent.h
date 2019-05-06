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
	//タイルセットの画像データの読み込み
	void SetTileSet(SDL_Texture* tileSet, int tileSetXNum, int tileSetYNum);
	//SpriteComponentのDraw関数のオーバーロード
	void Draw(SDL_Renderer* renderer, int id = -1);
private:
	SDL_Rect* GetDrawRect(int id);
	//タイルセットサイズ
	Vector2 tileSize;
	//タイルセットのx, y座標の個数
	int tileSetXNum;
	int tileSetYNum;
	//タイルセットデータ
	SDL_Texture* tileSetTexture;
};

