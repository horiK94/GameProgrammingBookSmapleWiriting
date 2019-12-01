// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "UIScreen.h"
#include <vector>

class HUD : public UIScreen
{
public:
	// (Lower draw order corresponds with further back)
	HUD(class Game* game);
	~HUD();

	void Update(float deltaTime) override;
	void Draw(class Shader* shader) override;
	
	//TargetComponent�̒ǉ�
	void AddTargetComponent(class TargetComponent* tc);
	void RemoveTargetComponent(class TargetComponent* tc);
protected:
	//�Փ˂��Ă��邩�ǂ����`�F�b�N����֐�
	void UpdateCrosshair(float deltaTime);
	//���[�U�[�̋P�_�̍X�V
	void UpdateRadar(float deltaTime);
	
	class Texture* mHealthBar;
	class Texture* mRadar;
	//�\�����̃e�N�X�`��
	class Texture* mCrosshair;
	//�G�𑨂����Ƃ��̏\�����̃e�N�X�`��
	class Texture* mCrosshairEnemy;
	class Texture* mBlipTex;
	class Texture* mRadarArrow;
	
	// All the target components in the game
	// �^�[�Q�b�g�c���R���|�[�l���g�̃|�C���^�z��
	std::vector<class TargetComponent*> mTargetComps;
	// 2D offsets of blips relative to radar
	//���[�_�[���S����P�_�ւ�2D���΃I�t�Z�b�g
	//���[�_�[�̍X�V���͂����̒l���X�V����
	//�w�i�`���A���̒l�����ƂɋP�_��`�悷��Ηǂ�
	std::vector<Vector2> mBlips;
	// Adjust range of radar and radius
	//���[�_�[�̕\���L���͈�(���͈̔̓M���M���̂Ƃ��́A���[�_�[�̋P�_��������M���M���̉����ɂ���Ƃ�������)(�ォ�猩�����[���h���W�̃T�C�Y)
	float mRadarRange;
	//���[�_�[�̋P�_�̕\����ł̗L�����a(�摜�T�C�Y�ɍ��킹��K�v������)(�X�N���[��2D�̃T�C�Y)
	float mRadarRadius;
	// Whether the crosshair targets an enemy
	//�\�������G�𑨂��Ă��邩
	bool mTargetEnemy;
};
