// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "PlaneActor.h"
#include "Game.h"
#include "Renderer.h"
#include "MeshComponent.h"
#include "BoxComponent.h"
#include "Mesh.h"

PlaneActor::PlaneActor(Game* game)
	:Actor(game)
{
	SetScale(10.0f);
	MeshComponent* mc = new MeshComponent(this);
	//Mesh* mesh = GetGame()->GetRenderer()->GetMesh("Assets/Plane.gpmesh");
	//gpmesh����K�v��Mesh�f�[�^�̎擾
	Mesh* mesh = GetGame()->GetRenderer()->GetMesh("Assets/Plane.gpmesh");
	mc->SetMesh(mesh);
	// Add collision box
	//mBox = new BoxComponent(this);
	//mBox->SetObjectBox(mesh->GetBox());
	mBox = new BoxComponent(this);
	mBox->SetObjectBox(mesh->GetBox());

	//�ǂ�game�ɕۑ�����(Actor�Ƃ̏Փ˔���̂���)
	game->AddPlane(this);
}

PlaneActor::~PlaneActor()
{
	GetGame()->RemovePlane(this);
}
