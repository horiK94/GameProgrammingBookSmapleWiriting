#include "TileMapComponent.h"
#include "Actor.h"

TileMapComponent::TileMapComponent(Actor* actor)
	:SpriteComponent::SpriteComponent(actor)
	,tileSize(Vector2::Zero)
	,tileSetXNum(0)
	,tileSetYNum(0)
	,tileSetTexture(nullptr)
{
}


TileMapComponent::~TileMapComponent()
{
}

void TileMapComponent::SetTileSet(SDL_Texture* tileSet, int tileXNum, int tileYNum)
{
	//Textureの設定(ここで画像のサイズは計算される)
	SetTexture(tileSet);

	tileSetXNum = tileXNum;
	tileSetYNum = tileYNum;

	//タイル1枚あたりのサイズを保存しておく
	tileSize.x = mTexWidth / tileXNum;
	tileSize.y = mTexHeight / tileYNum;
}

void TileMapComponent::Draw(SDL_Renderer* rendere, int id)
{
	if (mTexture)
	{
		SDL_Rect r;		//SDL_Rextのx, y座標は左上の座標のため、以下で補正を行う
// Scale the width/height by owner's scale
		r.w = static_cast<int>(tileSize.x * mOwner->GetScale());		//横幅*サイズ
		r.h = static_cast<int>(tileSize.y * mOwner->GetScale());		//縦幅*サイズ
		// Center the rectangle around the position of the owner
		r.x = static_cast<int>(mOwner->GetPosition().x - r.w / 2);		//Actorが保持しているmPositionは中心点の座標のため、修正しなければならない
		r.y = static_cast<int>(mOwner->GetPosition().y - r.h / 2);

		SDL_RenderCopyEx(
			rendere,
			mTexture,
			GetDrawRect(id),
			&r,
			-Math::ToDegrees(mOwner->GetRotation()),
			nullptr,
			SDL_FLIP_NONE
		);
	}
}

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