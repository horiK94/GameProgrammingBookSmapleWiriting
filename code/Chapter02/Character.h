#pragma once
#include "Actor.h"
#include "AnimSpriteComponent.h"
#include <string>

class Character :
	public Actor
{
public:
	Character(Game* game);
	~Character();
	virtual void UpdateActor(float deltaTime) override;
	void ProcessAnimKeyBoard(const uint8_t* state);
private:
	std::string animName;
	class AnimSpriteComponent* animSpriteComponent;
};

