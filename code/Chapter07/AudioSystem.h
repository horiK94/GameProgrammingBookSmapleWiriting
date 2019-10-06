// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include <unordered_map>
#include <string>
#include "SoundEvent.h"
#include "Math.h"

//FMOD�^�̑O���錾�����邱�ƂŁAfmod_studio.hpp�̃C���N���[�h�������
namespace FMOD
{
	class System;
	namespace Studio
	{
		class Bank;
		class EventDescription;
		class EventInstance;
		class System;
		class Bus;
	};
};

class AudioSystem
{
public:
	AudioSystem(class Game* game);
	~AudioSystem();

	bool Initialize();
	void Shutdown();

	// Load/unload banks
	void LoadBank(const std::string& name);
	void UnloadBank(const std::string& name);
	void UnloadAllBanks();

	SoundEvent PlayEvent(const std::string& name);

	void Update(float deltaTime);

	// For positional audio
	//���X�i�[�̐ݒ�
	void SetListener(const Matrix4& viewMatrix);
	//// Control buses
	float GetBusVolume(const std::string& name) const;
	bool GetBusPaused(const std::string& name) const;
	void SetBusVolume(const std::string& name, float volume);
	void SetBusPaused(const std::string& name, bool pause);
protected:
	//friend class SoundEvent;
	//GetEventInstance�|�C���^���擾���邽�߂̊֐�(protected�ɂ��Ă���̂́A�Ăяo�����ƂŃ|�C���^��G���Ăق����Ȃ�����)
	friend class SoundEvent;		//�T�E���h�C�x���g��friend�ɂ��āA�A�N�Z�X�ł���悤�ɂ���
	FMOD::Studio::EventInstance* GetEventInstance(unsigned int id);
private:
	// Tracks the next ID to use for event instances
	//�C�x���g�C���X�^���𐶐����邽�тɃC���N�������g���āA�O���[�o���Ȓl�ɂ��邽��
	static unsigned int sNextID;

	class Game* mGame;
	//���[�h���ꂽ�o���N�̘A�z�z��(�L�[�̓o���N�̃t�@�C����)(���[�h���ꂽ�o���N�Ȃ̂ŁA�o���N���̃C�x���g�����邱�Ƃ��ł���)
	std::unordered_map<std::string, FMOD::Studio::Bank*> mBanks;
	//�C�x���g������EventDescription�ւ̘A�z�z��(�L�[��FMOD���C�x���g�Ɋ��蓖�Ă����O=�p�X�̌`��(ex. event:/Explositon2D))
	std::unordered_map<std::string, FMOD::Studio::EventDescription*> mEvents;
	// Map of loaded banks
	//std::unordered_map<std::string, FMOD::Studio::Bank*> mBanks;
	//// Map of event name to EventDescription
	//std::unordered_map<std::string, FMOD::Studio::EventDescription*> mEvents;
	//// Map of event id to EventInstance
	//��������C�x���g�C���X�^���X�ւ̘A�z�z��
	std::unordered_map<unsigned int, FMOD::Studio::EventInstance*> mEventInstances;
	//// Map of buses
	//�o�X(�T�E���h�̃O���[�v)�̘A�z�z��
	std::unordered_map<std::string, FMOD::Studio::Bus*> mBuses;
	// FMOD studio �V�X�e��
	FMOD::Studio::System* mSystem;
	// FMOD Low-level system (�K�v���̂��߂�)
	FMOD::System* mLowLevelSystem;
};