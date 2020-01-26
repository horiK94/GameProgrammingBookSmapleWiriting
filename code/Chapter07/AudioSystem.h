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

//FMOD型の前方宣言をすることで、fmod_studio.hppのインクルードを避ける
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
	//リスナーの設定
	void SetListener(const Matrix4& viewMatrix);
	//// Control buses
	float GetBusVolume(const std::string& name) const;
	bool GetBusPaused(const std::string& name) const;
	void SetBusVolume(const std::string& name, float volume);
	void SetBusPaused(const std::string& name, bool pause);
protected:
	//friend class SoundEvent;
	//GetEventInstanceポインタを取得するための関数(protectedにしているのは、呼び出しもとでポインタを触ってほしくないから)
	friend class SoundEvent;		//サウンドイベントはfriendにして、アクセスできるようにした
	FMOD::Studio::EventInstance* GetEventInstance(unsigned int id);
private:
	// Tracks the next ID to use for event instances
	//イベントインスタンを生成するたびにインクリメントして、グローバルな値にするため
	static unsigned int sNextID;

	class Game* mGame;
	//ロードされたバンクの連想配列(キーはバンクのファイル名)(ロードされたバンクなので、バンク内のイベントを見ることができる)
	std::unordered_map<std::string, FMOD::Studio::Bank*> mBanks;
	//イベント名からEventDescriptionへの連想配列(キーはFMODがイベントに割り当てた名前=パスの形式(ex. event:/Explositon2D))
	std::unordered_map<std::string, FMOD::Studio::EventDescription*> mEvents;
	// Map of loaded banks
	//std::unordered_map<std::string, FMOD::Studio::Bank*> mBanks;
	//// Map of event name to EventDescription
	//std::unordered_map<std::string, FMOD::Studio::EventDescription*> mEvents;
	//// Map of event id to EventInstance
	//整数からイベントインスタンスへの連想配列
	std::unordered_map<unsigned int, FMOD::Studio::EventInstance*> mEventInstances;
	//// Map of buses
	//バス(サウンドのグループ)の連想配列
	std::unordered_map<std::string, FMOD::Studio::Bus*> mBuses;
	// FMOD studio システム
	FMOD::Studio::System* mSystem;
	// FMOD Low-level system (必要時のために)
	FMOD::System* mLowLevelSystem;
};