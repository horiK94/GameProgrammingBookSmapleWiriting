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
	//Texture�̐ݒ�(�����ŉ摜�̃T�C�Y�͌v�Z�����)
	SetTexture(tileSet);

	tileSetXNum = tileXNum;
	tileSetYNum = tileYNum;

	//�^�C��1��������̃T�C�Y��ۑ����Ă���
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
				//�`��ꏊ�̕`����s��
				int id = tileMapData[i][k];
				if (id == -1)
				{
					//�`����s��Ȃ�
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
			//id ��-1�̂Ƃ��͕`�悵�Ȃ�(��O����)
			return;
		}

		SDL_Rect r;		//SDL_Rext��x, y���W�͍���̍��W�̂��߁A�ȉ��ŕ␳���s��
// Scale the width/height by owner's scale
		r.w = static_cast<int>(tileSetSize.x * mOwner->GetScale());		//����*�T�C�Y
		r.h = static_cast<int>(tileSetSize.y * mOwner->GetScale());		//�c��*�T�C�Y
		// Center the rectangle around the position of the owner
		r.x = static_cast<int>(mOwner->GetPosition().x - (tileWidth * tileSetSize.x / 2) + x * r.w);		//Actor���ێ����Ă���mPosition�͒��S�_�̍��W�̂��߁A�C�����Ȃ���΂Ȃ�Ȃ�
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
	//0����n�܂�c, ���ɑ΂���id���v�Z
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
	//0����n�܂�c, ���ɑ΂���id���v�Z
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
��̂悤�ɏ����ƁArect�̃A�h���X���Ԃ����O�ɁA�Ԃ�l��rect�̎��������Ă��܂��̂Œ���(���܂������Ȃ��Ȃ�)
*/

void TileMapComponent::SetTileData(std::string fileName)
{
	 tileMapData = CSV::LoadCSV(fileName);
	 tileWidth = tileMapData.front().size();
	 tileHeight = tileMapData.size();
}