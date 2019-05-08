#include "TileMapComponent.h"
#include "Actor.h"
#include <fstream>
#include "CSV.h"

TileMapComponent::TileMapComponent(Actor* actor, int drawOrder)
	:SpriteComponent::SpriteComponent(actor, drawOrder)
	,tileSetSize(Vector2::Zero)
	,tileSetXNum(0)
	,tileSetYNum(0)
{
}


TileMapComponent::~TileMapComponent()
{
}

void TileMapComponent::SetTileSet(SDL_Texture* tileSet, Vector2 screenSize, int tileXNum, int tileYNum)
{
	//Textureの設定(ここで画像のサイズは計算される)
	SetTexture(tileSet);

	tileSetXNum = tileXNum;
	tileSetYNum = tileYNum;

	//タイル1枚あたりのサイズを保存しておく
	tileSetSize.x = mTexWidth / tileXNum;
	tileSetSize.y = mTexHeight / tileYNum;

	this->screenSize = screenSize;
}

void TileMapComponent::Draw(SDL_Renderer* rendere)
{
	if (mTexture)
	{
		for (int i = 0; i < tileHeight; i++)
		{
			for (int k = 0; k < tileWidth; k++)
			{
				//描画場所の描画を行う
				int id = tileMapData[i][k];
				if (id == -1)
				{
					//描画を行わない
					continue;
				}
				DrawTile(rendere, k, i, id);
			}
		}
	}
}

void TileMapComponent::DrawTile(SDL_Renderer* renderer, int x, int y, int id)
{
	if (mTexture)
	{
		if (id == -1)
		{
			//id が-1のときは描画しない(例外処理)
			return;
		}

		SDL_Rect r;		//SDL_Rextのx, y座標は左上の座標のため、以下で補正を行う
// Scale the width/height by owner's scale
		r.w = static_cast<int>(tileSetSize.x * mOwner->GetScale());		//横幅*サイズ
		r.h = static_cast<int>(tileSetSize.y * mOwner->GetScale());		//縦幅*サイズ
		// Center the rectangle around the position of the owner
		r.x = static_cast<int>(mOwner->GetPosition().x - (tileWidth * tileSetSize.x / 2) + x * r.w);		//Actorが保持しているmPositionは中心点の座標のため、修正しなければならない
		r.y = static_cast<int>(mOwner->GetPosition().y - (tileHeight * tileSetSize.y / 2) + y * r.h);

		SDL_Rect tileRect;
		GetDrawSetMapRect(&tileRect, id);

		SDL_RenderCopyEx(
			renderer,
			mTexture,
			&tileRect,
			&r,
			-Math::ToDegrees(mOwner->GetRotation()),
			nullptr,
			SDL_FLIP_NONE
		);
	}
}

void TileMapComponent::GetDrawSetMapRect(SDL_Rect* rect, int id)
{
	//0から始まる縦, 横に対するidを計算
	int xNumber = id % tileSetXNum;
	int yNumber = id / tileSetXNum;
	if (yNumber < 0 || yNumber >= tileSetYNum)
	{
		SDL_Log("id: %d is failed.", id);
	}

	(*rect).w = static_cast<int>(tileSetSize.x);
	(*rect).h = static_cast<int>(tileSetSize.y);
	(*rect).x = static_cast<int>(xNumber * tileSetSize.x);
	(*rect).y = static_cast<int>(yNumber * tileSetSize.y);
}
/*

SDL_Rect* TileMapComponent::GetDrawRect(int id)
{
	//0から始まる縦, 横に対するidを計算
	int xNumber = id % tileSetXNum;
	int yNumber = id / tileSetXNum;
	if (yNumber < 0 || yNumber >= tileSetYNum)
	{
		SDL_Log("id: %d is failed.", id);
		return nullptr;
	}

	SDL_Rect rect;
	rect.w = static_cast<int>(tileSize.x);
	rect.h = static_cast<int>(tileSize.y);
	rect.x = static_cast<int>(xNumber * tileSize.x);
	rect.y = static_cast<int>(yNumber * tileSize.y);

	return &rect;
} 
上のように書くと、rectのアドレスが返される前に、返り値のrectの寿命が来てしまうので注意(うまく動かなくなる)
*/

void TileMapComponent::SetTileData(std::string fileName)
{
	 tileMapData = CSV::LoadCSV(fileName);
	 tileWidth = tileMapData.front().size();
	 tileHeight = tileMapData.size();
}