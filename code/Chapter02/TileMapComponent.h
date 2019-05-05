#pragma once
#include "SpriteComponent.h"

class TileMapComponent :
	public SpriteComponent
{
public:
	TileMapComponent(Actor* actor);
	~TileMapComponent();
};

