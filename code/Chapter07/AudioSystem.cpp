// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "AudioSystem.h"
#include <SDL/SDL_log.h>
#include <fmod_studio.hpp>
#include <fmod_errors.h>
#include <vector>

unsigned int AudioSystem::sNextID = 0;

AudioSystem::AudioSystem(Game* game)
	:mGame(game)
	, mSystem(nullptr)
	, mLowLevelSystem(nullptr)
{
}

AudioSystem::~AudioSystem()
{
}

bool AudioSystem::Initialize()
{
	// Initialize debug logging
	//�G���[���O�ݒ�
	//FMOD::Debug_Initialize(
	//	FMOD_DEBUG_LEVEL_ERROR, // Log only errors
	//	FMOD_DEBUG_MODE_TTY // Output to stdout
	//);
	FMOD::Debug_Initialize(
		//�G���[�������O���Ƃ�
		FMOD_DEBUG_LEVEL_ERROR,
		//stdout�ɏo��
		FMOD_DEBUG_MODE_TTY
	);

	// Create FMOD studio system object
	//FMOD studio�V�X�e���̃C���X�^���X�쐬
	//FMOD_RESULT result;		//�C���X�^���X�쐬���ʂ�ۑ�����ϐ�
	//result = FMOD::Studio::System::create(&mSystem);		//Fmod studio�V�X�e���쐬
	//if (result != FMOD_OK)
	//{
	//	//���ʂ�OK�łȂ�������A�G���[��f��
	//	SDL_Log("Failed to create FMOD system: %s", FMOD_ErrorString(result));		//FMOD_ErrorString(): �G���[�R�[�h��ǂ߂�`�ŕԂ�
	//	return false;
	//}
	//FMOD Studio�V�X�e���̏�����
	FMOD_RESULT result;
	//�V�X�e���̍쐬(mSystem�ɍ쐬�������̂̃|�C���^������)
	result = FMOD::Studio::System::create(&mSystem);
	if (result != FMOD_OK)
	{
		SDL_Log("FMOD�V�X�e���̍쐬�Ɏ��s���܂���: %s", FMOD_ErrorString(result));		//�G���[�R�[�h��l�Ԃ��ǂ߂�`���ɂ���悤

		return false;
	}

	//�V�X�e����������珉��������
	result = mSystem->initialize(
		512,		//�`���l���ő吔
		FMOD_STUDIO_INIT_NORMAL,		//FMOD Studio API�̂ӂ�܂�(����̓f�t�H���g�l)
		FMOD_INIT_NORMAL,		//FMOD Low Level API�̂ӂ�܂�(����̓f�t�H���g�l)
		nullptr		//�ǉ��̃h���C�o�[���g�p����Ƃ��ɐݒ肷��(�g��Ȃ��ꍇ��nullptr)
	);
	if (result != FMOD_OK)
	{
		SDL_Log("FMOD�V�X�e���̏������Ɏ��s���܂���: %s", FMOD_ErrorString(result));		//�G���[�R�[�h��l�Ԃ��ǂ߂�`���ɂ���悤

		return false;
	}

	//Low Level�V�X�e���|�C���^�̎擾
	mSystem->getLowLevelSystem(&mLowLevelSystem);
	mLowLevelSystem->set3DSettings(
		10.0f,
		5.0f,
		1.0f
	);

	//// Initialize FMOD studio system
	////FMOD studio�V�X�e���̏�����
	//result = mSystem->initialize(
	//	//�ő哯��������(�T�E���h�`���l�����̍ő�)
	//	512, // Max number of concurrent sounds
	//	//FMOD studio�̂ӂ�܂��ݒ�(�f�t�H���g�ݒ�)
	//	FMOD_STUDIO_INIT_NORMAL, // Use default settings
	//	//FMOD Lowlevel API�̂ӂ�܂��ݒ�(�f�t�H���g�ݒ�)
	//	FMOD_INIT_NORMAL, // Use default settings
	//	//�ǉ��̃h���C�u�f�[�^���g���Ƃ��Ɏg�p
	//	nullptr // Usually null
	//);
	//if (result != FMOD_OK)
	//{
	//	SDL_Log("Failed to initialize FMOD system: %s", FMOD_ErrorString(result));
	//	return false;
	//}

	//// Save the low-level system pointer
	////low level�V�X�e���|�C���^�̎擾
	//mSystem->getLowLevelSystem(&mLowLevelSystem);

	//// Load the master banks (strings first)
	//LoadBank("Assets/Master Bank.strings.bank");
	//LoadBank("Assets/Master Bank.bank");

	//FMOD Studio�Ƀf�t�H���g�ŗp�ӂ���Ă���2�̃t�@�C�������[�h����(2�̃}�X�^�[�o���N�t�@�C�������[�h���Ȃ��Ƒ��̃o���N��C�x���g�ɃA�N�Z�X�ł��Ȃ�)
	//(string����炵��)
	//FMOD Studio�v���W�F�N�g�̂��ׂẴC�x���g�Ƃ��̑��̏�񂪐l�Ԃɓǂ߂閼�O�Ƃ��ē����Ă���B��������[�h���Ȃ��ƁA���O�ŃA�N�Z�X�ł����Aguid�ŃA�N�Z�X���Ȃ��ƂȂ�Ȃ�(�I�v�V����)
	LoadBank("Assets/Master Bank.strings.bank");
	LoadBank("Assets/Master Bank.bank");

	return true;
}

void AudioSystem::Shutdown()
{
	if (mSystem)		//mSystem�����݂���Ȃ�
	{
		mSystem->release();		//���
	}
	// Unload all banks
	//UnloadAllBanks();
	//// Shutdown FMOD system
	//if (mSystem)
	//{
	//	mSystem->release();
	//}
}

void AudioSystem::LoadBank(const std::string& name)
{
	//���d�ɓǂݍ��܂Ȃ��悤�`�F�b�N
	if (mBanks.find(name) != mBanks.end())
	{
		return;
	}

	//�o���N�����[�h(�o���N�̃��[�h�������ƃT���v���f�[�^�̓��[�h����Ă��炸�AEventDescription�ɂ��A�N�Z�X���e�ՂłȂ����Ƃɒ���)
	FMOD::Studio::Bank* bank = nullptr;		//���[�h�����o���N
	FMOD_RESULT result = mSystem->loadBankFile(
		name.c_str(),		//�t�@�C�����������͋�
		FMOD_STUDIO_LOAD_BANK_NORMAL,		//���ʂ̓ǂݍ���
		&bank		//�o���N�ւ̃|�C���^��ۑ�
	);

	const int maxPathLength = 512;
	if (result == FMOD_OK)
	{
		//�o���N��A�z�z��ɒǉ�
		mBanks.emplace(name, bank);
		//�X�g���[�~���O�f�[�^�ȊO�̃T���v���f�[�^�����ׂă��[�h
		bank->loadSampleData();
		//���̃o���N�ɂ���C�x���g�̐����擾����ϐ�
		int numEvents = 0;
		//�o���N�ɂ���C�x���g�̐�����������
		bank->getEventCount(&numEvents);
		if (numEvents > 0)
		{
			//�o���N�ɂ���C�x���g�̋L�q�q(�^: EventDesription)�̃��X�g�̎擾
			std::vector<FMOD::Studio::EventDescription*> events(numEvents);		//���X�g�̐� = numEvents
			bank->getEventList(events.data(), numEvents, &numEvents);
			char eventName[maxPathLength];
			for (int i = 0; i < numEvents; i++)
			{
				FMOD::Studio::EventDescription* e = events[i];
				//���̃C�x���g�̃p�X���擾
				e->getPath(eventName, maxPathLength, nullptr);
				//�C�x���g�A�z�z��ɒǉ�
				mEvents.emplace(eventName, e);
			}
		}
		//�o�X�̌��̕\��
		int numBuses = 0;
		bank->getBusCount(&numBuses);
		if (numBuses > 0)
		{
			std::vector<FMOD::Studio::Bus*> buses(numBuses);
			bank->getBusList(buses.data(), numBuses, &numBuses);
			char busName[512];
			for (int i = 0; i < numBuses; i++)
			{
				FMOD::Studio::Bus* bus = buses[i];
				bus->getPath(busName, 512, nullptr);
				mBuses.emplace(busName, bus);
			}
		}
	}

	//// Prevent double-loading
	//if (mBanks.find(name) != mBanks.end())
	//{
	//	return;
	//}

	//// Try to load bank
	//FMOD::Studio::Bank* bank = nullptr;
	//FMOD_RESULT result = mSystem->loadBankFile(
	//	name.c_str(), // File name of bank
	//	FMOD_STUDIO_LOAD_BANK_NORMAL, // Normal loading
	//	&bank // Save pointer to bank
	//);

	//const int maxPathLength = 512;
	//if (result == FMOD_OK)
	//{
	//	// Add bank to map
	//	mBanks.emplace(name, bank);
	//	// Load all non-streaming sample data
	//	bank->loadSampleData();
	//	// Get the number of events in this bank
	//	int numEvents = 0;
	//	bank->getEventCount(&numEvents);
	//	if (numEvents > 0)
	//	{
	//		// Get list of event descriptions in this bank
	//		std::vector<FMOD::Studio::EventDescription*> events(numEvents);
	//		bank->getEventList(events.data(), numEvents, &numEvents);
	//		char eventName[maxPathLength];
	//		for (int i = 0; i < numEvents; i++)
	//		{
	//			FMOD::Studio::EventDescription* e = events[i];
	//			// Get the path of this event (like event:/Explosion2D)
	//			e->getPath(eventName, maxPathLength, nullptr);
	//			// Add to event map
	//			mEvents.emplace(eventName, e);
	//		}
	//	}
	//	// Get the number of buses in this bank
	//	int numBuses = 0;
	//	bank->getBusCount(&numBuses);
	//	if (numBuses > 0)
	//	{
	//		// Get list of buses in this bank
	//		std::vector<FMOD::Studio::Bus*> buses(numBuses);
	//		bank->getBusList(buses.data(), numBuses, &numBuses);
	//		char busName[512];
	//		for (int i = 0; i < numBuses; i++)
	//		{
	//			FMOD::Studio::Bus* bus = buses[i];
	//			// Get the path of this bus (like bus:/SFX)
	//			bus->getPath(busName, 512, nullptr);
	//			// Add to buses map
	//			mBuses.emplace(busName, bus);
	//		}
	//	}
	//}
}

void AudioSystem::UnloadBank(const std::string& name)
{
	// Ignore if not loaded
	auto iter = mBanks.find(name);
	if (iter == mBanks.end())
	{
		return;
	}

	// First we need to remove all events from this bank
	FMOD::Studio::Bank* bank = iter->second;
	int numEvents = 0;
	bank->getEventCount(&numEvents);
	//�܂��ŏ���mEvents�ɂ���Y���̃C�x���g���폜����
	if (numEvents > 0)
	{
		// Get event descriptions for this bank
		std::vector<FMOD::Studio::EventDescription*> events(numEvents);
		// Get list of events
		bank->getEventList(events.data(), numEvents, &numEvents);
		char eventName[512];
		for (int i = 0; i < numEvents; i++)
		{
			FMOD::Studio::EventDescription* e = events[i];
			// Get the path of this event
			e->getPath(eventName, 512, nullptr);
			// Remove this event
			auto eventi = mEvents.find(eventName);
			if (eventi != mEvents.end())
			{
				mEvents.erase(eventi);
			}
		}
	}
	// Get the number of buses in this bank
	int numBuses = 0;
	bank->getBusCount(&numBuses);
	if (numBuses > 0)
	{
		// Get list of buses in this bank
		std::vector<FMOD::Studio::Bus*> buses(numBuses);
		//�Y������o���N�̃p�X���擾(���̊֐����g�p����ƁA�O���[�o���~�L�T�[���̃o�X�̃n���h�����擾�ł��܂��B)
		//�o�X���ĂȂ񂼁B�B�B
		bank->getBusList(buses.data(), numBuses, &numBuses);
		char busName[512];
		for (int i = 0; i < numBuses; i++)
		{
			FMOD::Studio::Bus* bus = buses[i];
			// Get the path of this bus (like bus:/SFX)
			bus->getPath(busName, 512, nullptr);
			// Remove this bus
			auto busi = mBanks.find(busName);
			if (busi != mBanks.end())
			{
				mBanks.erase(busi);
			}
		}
	}

	// Unload sample data and bank
	bank->unloadSampleData();
	bank->unload();
	// Remove from banks map
	mBanks.erase(iter);
}

void AudioSystem::UnloadAllBanks()
{
	for (auto& iter : mBanks)
	{
		// Unload the sample data, then the bank itself
		//�S�T���v���f�[�^�̍폜
		iter.second->unloadSampleData();
		iter.second->unload();
	}
	mBanks.clear();
	// No banks means no events
	mEvents.clear();
}

SoundEvent AudioSystem::PlayEvent(const std::string& name)
{
	unsigned int retID = 0;
	auto iter = mEvents.find(name);
	if (iter != mEvents.end())
	{
		// Create instance of event
		FMOD::Studio::EventInstance* event = nullptr;
		iter->second->createInstance(&event);
		if (event)
		{
			// Start the event instance
			//�Đ����n�܂�
			event->start();
			// Get the next id, and add to map
			sNextID++;
			retID = sNextID;
			//PlayEvent��id�̘A�z�z��ɒǉ�
			mEventInstances.emplace(retID, event);
		}
	}
	//id�����蓖�Ă�soundEvent��Ԃ�
	return SoundEvent(this, retID);
}

void AudioSystem::Update(float deltaTime)
{
	//�s�v�ɂȂ���eventInstance���N���[���A�b�v����

	//�X�g�b�v�ɂȂ��Ă���eventInstance��������(�����Ă��������Ȃ����Pause�ɂ��Ă���)
	std::vector<unsigned int> done;
	for (auto& iter : mEventInstances)
	{
		FMOD::Studio::EventInstance* e = iter.second;
		//�C�x���g�̏�Ԃ��擾
		FMOD_STUDIO_PLAYBACK_STATE state;
		e->getPlaybackState(&state);
		if (state == FMOD_STUDIO_PLAYBACK_STOPPED)
		{
			//�C�x���g�C���X�^���X����~��Ԃ̏ꍇ
			e->release();	//���
			done.emplace_back(iter.first);
		}
	}

	//�I�������C�x���g�C���X�^���X��A�z�z�񂩂����
	for (auto id : done)
	{
		mEventInstances.erase(id);
	}

	//// Find any stopped event instances
	//std::vector<unsigned int> done;
	//for (auto& iter : mEventInstances)
	//{
	//	FMOD::Studio::EventInstance* e = iter.second;
	//	// Get the state of this event
	//	FMOD_STUDIO_PLAYBACK_STATE state;
	//	e->getPlaybackState(&state);
	//	if (state == FMOD_STUDIO_PLAYBACK_STOPPED)
	//	{
	//		// Release the event and add id to done
	//		e->release();
	//		done.emplace_back(iter.first);
	//	}
	//}

	//// Remove done event instances from map
	//for (auto id : done)
	//{
	//	mEventInstances.erase(id);
	//}

	//// Update FMOD
	//mSystem->update();

	//FMOD�̍X�V(3D�I�[�f�B�I�̍Čv�Z��������K�v�����邽��)
	mSystem->update();
}

namespace
{
	FMOD_VECTOR VecToFMOD(const Vector3& in)
	{
		// Convert from our coordinates (+x forward, +y right, +z up)
		// to FMOD (+z forward, +x right, +y up)
		FMOD_VECTOR v;
		v.x = in.y;
		v.y = in.z;
		v.z = in.x;
		return v;
	}
}

//���X�i�[�̒ǉ�
void AudioSystem::SetListener(const Matrix4& viewMatrix)
{
	//// Invert the view matrix to get the correct vectors
	//Matrix4 invView = viewMatrix;
	//invView.Invert();
	//FMOD_3D_ATTRIBUTES listener;
	//// Set position, forward, up
	//listener.position = VecToFMOD(invView.GetTranslation());
	//// In the inverted view, third row is forward
	//listener.forward = VecToFMOD(invView.GetZAxis());
	//// In the inverted view, second row is up
	//listener.up = VecToFMOD(invView.GetYAxis());
	//// Set velocity to zero (fix if using Doppler effect)
	//listener.velocity = { 0.0f, 0.0f, 0.0f };
	//// Send to FMOD
	//mSystem->setListenerAttributes(0, &listener);

	//���X�i�[�̏��Ƃ��ĕK�v�Ȃ̂̓J�����̈ʒu�ƕ���
	//�����Ƃ��ăr���[�s����󂯎��A���[���h�s��ɖ߂��Ď擾���邱�Ƃ��l����
	Matrix4 invView = viewMatrix;
	invView.Invert();
	//FMOD_3D_ATTRIBUTES: �ʒu�A���x�A�������L�q����\���B
	FMOD_3D_ATTRIBUTES listener;
	//�ʒu�̃Z�b�g
	listener.position = VecToFMOD(invView.GetTranslation());
	//�t�r���[�ł͑�3�s���O�����ƂȂ�
	listener.forward = VecToFMOD(invView.GetZAxis());
	//�t�r���[�ł͑�2�s�񂪏����
	listener.up = VecToFMOD(invView.GetYAxis());
	//���x��0��(�h�b�v���[���ʂ�ݒ肷��Ƃ��ɑΉ�)
	listener.velocity = { 0.f, 0.f, 0.f };
	//FMOD�ɑ���(0�̓��X�i�[��1�l�̎��Ɏw��)
	mSystem->setListenerAttributes(0, &listener);
}

float AudioSystem::GetBusVolume(const std::string& name) const
{
	float retVal = 0.0f;
	const auto iter = mBuses.find(name);
	if (iter != mBuses.end())
	{
		iter->second->getVolume(&retVal);
	}
	return retVal;
}

bool AudioSystem::GetBusPaused(const std::string& name) const
{
	bool retVal = false;
	const auto iter = mBuses.find(name);
	if (iter != mBuses.end())
	{
		iter->second->getPaused(&retVal);
	}
	return retVal;
}

void AudioSystem::SetBusVolume(const std::string& name, float volume)
{
	auto iter = mBuses.find(name);
	if (iter != mBuses.end())
	{
		iter->second->setVolume(volume);
	}
}

void AudioSystem::SetBusPaused(const std::string& name, bool pause)
{
	auto iter = mBuses.find(name);
	if (iter != mBuses.end())
	{
		iter->second->setPaused(pause);
	}
}

FMOD::Studio::EventInstance* AudioSystem::GetEventInstance(unsigned int id)
{
	FMOD::Studio::EventInstance* event = nullptr;
	auto iter = mEventInstances.find(id);
	if (iter != mEventInstances.end())
	{
		event = iter->second;
	}
	return event;
}
