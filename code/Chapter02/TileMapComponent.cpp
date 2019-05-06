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
	//Texture�̐ݒ�(�����ŉ摜�̃T�C�Y�͌v�Z�����)
	SetTexture(tileSet);

	tileSetXNum = tileXNum;
	tileSetYNum = tileYNum;

	//�^�C��1��������̃T�C�Y��ۑ����Ă���
	tileSize.x = mTexWidth / tileXNum;
	tileSize.y = mTexHeight / tileYNum;
}

void TileMapComponent::Draw(SDL_Renderer* rendere, int id)
{
	if (mTexture)
	{
		SDL_Rect r;		//SDL_Rext��x, y���W�͍���̍��W�̂��߁A�ȉ��ŕ␳���s��
// Scale the width/height by owner's scale
		r.w = static_cast<int>(tileSize.x * mOwner->GetScale());		//����*�T�C�Y
		r.h = static_cast<int>(tileSize.y * mOwner->GetScale());		//�c��*�T�C�Y
		// Center the rectangle around the position of the owner
		r.x = static_cast<int>(mOwner->GetPosition().x - r.w / 2);		//Actor���ێ����Ă���mPosition�͒��S�_�̍��W�̂��߁A�C�����Ȃ���΂Ȃ�Ȃ�
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