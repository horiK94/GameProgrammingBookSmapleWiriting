// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "Component.h"

//ターゲット把握コンポーネント
class TargetComponent : public Component
{
public:
	TargetComponent(class Actor* owner);
	~TargetComponent();
};
