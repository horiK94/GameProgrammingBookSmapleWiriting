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
	//タイルセットの画像データの読み込み
	void SetTileSet(SDL_Texture* tileSet, Vector2 screenSize, int tileXNum, int tileYNum);
	void Draw(SDL_Renderer* renderer) override;
	void SetTileData(std::string fileName);
private:
	void GetDrawSetMapRect(SDL_Rect* rect, int id);		//指定のidのタイルマップRectデータの取得
	void DrawTile(SDL_Renderer* renderer, int x, int y, int id);		//タイルの描画

	//縦, 横の表示個数
	int tileWidth;
	int tileHeight;

	//タイルセットサイズ
	Vector2 tileSetSize;
	//タイルセットのx, y座標の個数
	int tileSetXNum;
	int tileSetYNum;
	//タイルマップ情報
	std::vector<std::vector<int>> tileMapData;

	Vector2 screenSize;
};

