#include "Character.h"
#include "AnimSpriteComponent.h"
#include "Game.h"

Character::Character(Game* game)
	:Actor(game)
{
	AnimSpriteComponent* asc = new AnimSpriteComponent(this);
	animSpriteComponent = asc;
	std::vector<SDL_Texture*> walking =
	{
		//include "Game.h"しないと、不完全クラス型へのポインターは使用できないといわれるので注意
		game->GetTexture("Assets/Character01.png"),
		game->GetTexture("Assets/Character02.png"),
		game->GetTexture("Assets/Character03.png"),
		game->GetTexture("Assets/Character04.png"),
		game->GetTexture("Assets/Character05.png"),
		game->GetTexture("Assets/Character06.png"),
	};
	std::vector<SDL_Texture*> jumping =
	{
		game->GetTexture("Assets/Character07.png"),
		game->GetTexture("Assets/Character08.png"),
		game->GetTexture("Assets/Character09.png"),
		game->GetTexture("Assets/Character10.png"),
		game->GetTexture("Assets/Character11.png"),
		game->GetTexture("Assets/Character12.png"),
		game->GetTexture("Assets/Character13.png"),
		game->GetTexture("Assets/Character14.png"),
		game->GetTexture("Assets/Character15.png"),
	};
	std::vector<SDL_Texture*> punch =
	{
		game->GetTexture("Assets/Character16.png"),
		game->GetTexture("Assets/Character17.png"),
		game->GetTexture("Assets/Character18.png"),
	};
	asc->SetAnimTextures("walking", walking);
	asc->SetAnimTextures("jumping", jumping);
	asc->SetAnimTextures("punch", punch);

	asc->ChangeAnim("walking");
}

void Character::UpdateActor(float deltaTime)
{
	Actor::UpdateActor(deltaTime);

}

void Character::ProcessAnimKeyBoard(const uint8_t* state)
{
	if (state[SDL_SCANCODE_1])
	{
		animName = "walking";
		animSpriteComponent->ChangeAnim(animName);
	}
	if (state[SDL_SCANCODE_2])
	{
		animName = "jumping";
		animSpriteComponent->ChangeAnim(animName);
	}
	if (state[SDL_SCANCODE_3])
	{
		animName = "punch";
		animSpriteComponent->ChangeAnim(animName);
	}
}

Character::~Character()
{
}
