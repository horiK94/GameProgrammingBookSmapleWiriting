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
	//エラーログ設定
	//FMOD::Debug_Initialize(
	//	FMOD_DEBUG_LEVEL_ERROR, // Log only errors
	//	FMOD_DEBUG_MODE_TTY // Output to stdout
	//);
	FMOD::Debug_Initialize(
		//エラーだけログをとる
		FMOD_DEBUG_LEVEL_ERROR,
		//stdoutに出力
		FMOD_DEBUG_MODE_TTY
	);

	// Create FMOD studio system object
	//FMOD studioシステムのインスタンス作成
	//FMOD_RESULT result;		//インスタンス作成結果を保存する変数
	//result = FMOD::Studio::System::create(&mSystem);		//Fmod studioシステム作成
	//if (result != FMOD_OK)
	//{
	//	//結果がOKでなかったら、エラーを吐く
	//	SDL_Log("Failed to create FMOD system: %s", FMOD_ErrorString(result));		//FMOD_ErrorString(): エラーコードを読める形で返す
	//	return false;
	//}
	//FMOD Studioシステムの初期化
	FMOD_RESULT result;
	//システムの作成(mSystemに作成したもののポインタが入る)
	result = FMOD::Studio::System::create(&mSystem);
	if (result != FMOD_OK)
	{
		SDL_Log("FMODシステムの作成に失敗しました: %s", FMOD_ErrorString(result));		//エラーコードを人間が読める形式にするよう

		return false;
	}

	//システムを作ったら初期化する
	result = mSystem->initialize(
		512,		//チャネル最大数
		FMOD_STUDIO_INIT_NORMAL,		//FMOD Studio APIのふるまい(今回はデフォルト値)
		FMOD_INIT_NORMAL,		//FMOD Low Level APIのふるまい(今回はデフォルト値)
		nullptr		//追加のドライバーを使用するときに設定する(使わない場合はnullptr)
	);
	if (result != FMOD_OK)
	{
		SDL_Log("FMODシステムの初期化に失敗しました: %s", FMOD_ErrorString(result));		//エラーコードを人間が読める形式にするよう

		return false;
	}

	//Low Levelシステムポインタの取得
	mSystem->getLowLevelSystem(&mLowLevelSystem);
	mLowLevelSystem->set3DSettings(
		10.0f,
		5.0f,
		1.0f
	);

	//// Initialize FMOD studio system
	////FMOD studioシステムの初期化
	//result = mSystem->initialize(
	//	//最大同時発声数(サウンドチャネル数の最大)
	//	512, // Max number of concurrent sounds
	//	//FMOD studioのふるまい設定(デフォルト設定)
	//	FMOD_STUDIO_INIT_NORMAL, // Use default settings
	//	//FMOD Lowlevel APIのふるまい設定(デフォルト設定)
	//	FMOD_INIT_NORMAL, // Use default settings
	//	//追加のドライブデータを使うときに使用
	//	nullptr // Usually null
	//);
	//if (result != FMOD_OK)
	//{
	//	SDL_Log("Failed to initialize FMOD system: %s", FMOD_ErrorString(result));
	//	return false;
	//}

	//// Save the low-level system pointer
	////low levelシステムポインタの取得
	//mSystem->getLowLevelSystem(&mLowLevelSystem);

	//// Load the master banks (strings first)
	//LoadBank("Assets/Master Bank.strings.bank");
	//LoadBank("Assets/Master Bank.bank");

	//FMOD Studioにデフォルトで用意されている2つのファイルをロードする(2つのマスターバンクファイルをロードしないと他のバンクやイベントにアクセスできない)
	//(stringが先らしい)
	//FMOD Studioプロジェクトのすべてのイベントとその他の情報が人間に読める名前として入っている。これをロードしないと、名前でアクセスできず、guidでアクセスしないとならない(オプション)
	LoadBank("Assets/Master Bank.strings.bank");
	LoadBank("Assets/Master Bank.bank");

	return true;
}

void AudioSystem::Shutdown()
{
	if (mSystem)		//mSystemが存在するなら
	{
		mSystem->release();		//解放
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
	//多重に読み込まないようチェック
	if (mBanks.find(name) != mBanks.end())
	{
		return;
	}

	//バンクをロード(バンクのロードだけだとサンプルデータはロードされておらず、EventDescriptionにもアクセスが容易でないことに注意)
	FMOD::Studio::Bank* bank = nullptr;		//ロードしたバンク
	FMOD_RESULT result = mSystem->loadBankFile(
		name.c_str(),		//ファイル名もしくは空白
		FMOD_STUDIO_LOAD_BANK_NORMAL,		//普通の読み込み
		&bank		//バンクへのポインタを保存
	);

	const int maxPathLength = 512;
	if (result == FMOD_OK)
	{
		//バンクを連想配列に追加
		mBanks.emplace(name, bank);
		//ストリーミングデータ以外のサンプルデータをすべてロード
		bank->loadSampleData();
		//このバンクにあるイベントの数を取得する変数
		int numEvents = 0;
		//バンクにあるイベントの数を所得する
		bank->getEventCount(&numEvents);
		if (numEvents > 0)
		{
			//バンクにあるイベントの記述子(型: EventDesription)のリストの取得
			std::vector<FMOD::Studio::EventDescription*> events(numEvents);		//リストの数 = numEvents
			bank->getEventList(events.data(), numEvents, &numEvents);
			char eventName[maxPathLength];
			for (int i = 0; i < numEvents; i++)
			{
				FMOD::Studio::EventDescription* e = events[i];
				//このイベントのパスを取得
				e->getPath(eventName, maxPathLength, nullptr);
				//イベント連想配列に追加
				mEvents.emplace(eventName, e);
			}
		}
		//バスの個数の表示
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
	//まず最初にmEventsにある該当のイベントを削除する
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
		//該当するバンクのパスを取得(この関数を使用すると、グローバルミキサー内のバスのハンドルを取得できます。)
		//バスってなんぞ。。。
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
		//全サンプルデータの削除
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
			//再生が始まる
			event->start();
			// Get the next id, and add to map
			sNextID++;
			retID = sNextID;
			//PlayEventとidの連想配列に追加
			mEventInstances.emplace(retID, event);
		}
	}
	//idを割り当てたsoundEventを返す
	return SoundEvent(this, retID);
}

void AudioSystem::Update(float deltaTime)
{
	//不要になったeventInstanceをクリーンアップする

	//ストップになっているeventInstanceを見つける(消しておきたくなければPauseにしておく)
	std::vector<unsigned int> done;
	for (auto& iter : mEventInstances)
	{
		FMOD::Studio::EventInstance* e = iter.second;
		//イベントの状態を取得
		FMOD_STUDIO_PLAYBACK_STATE state;
		e->getPlaybackState(&state);
		if (state == FMOD_STUDIO_PLAYBACK_STOPPED)
		{
			//イベントインスタンスが停止状態の場合
			e->release();	//解放
			done.emplace_back(iter.first);
		}
	}

	//終了したイベントインスタンスを連想配列から消す
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

	//FMODの更新(3Dオーディオの再計算等をする必要があるため)
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

//リスナーの追加
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

	//リスナーの情報として必要なのはカメラの位置と方向
	//引数としてビュー行列を受け取り、ワールド行列に戻して取得することを考える
	Matrix4 invView = viewMatrix;
	invView.Invert();
	//FMOD_3D_ATTRIBUTES: 位置、速度、方向を記述する構造。
	FMOD_3D_ATTRIBUTES listener;
	//位置のセット
	listener.position = VecToFMOD(invView.GetTranslation());
	//逆ビューでは第3行が前方向となる
	listener.forward = VecToFMOD(invView.GetZAxis());
	//逆ビューでは第2行列が上方向
	listener.up = VecToFMOD(invView.GetYAxis());
	//速度は0で(ドップラー効果を設定するときに対応)
	listener.velocity = { 0.f, 0.f, 0.f };
	//FMODに送る(0はリスナーが1人の時に指定)
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
