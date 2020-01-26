// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "SoundEvent.h"
#include "AudioSystem.h"
#include <fmod_studio.hpp>

SoundEvent::SoundEvent(class AudioSystem* system, unsigned int id)
	:mSystem(system)
	,mID(id)
{
}

SoundEvent::SoundEvent()
	:mSystem(nullptr)
	,mID(0)
{
}

bool SoundEvent::IsValid()
{
	return (mSystem && mSystem->GetEventInstance(mID) != nullptr);
}

void SoundEvent::Restart()
{
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
	if (event)
	{
		event->start();
	}
}

void SoundEvent::Stop(bool allowFadeOut /* true */)
{
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
	if (event)
	{
		FMOD_STUDIO_STOP_MODE mode = allowFadeOut ?
			FMOD_STUDIO_STOP_ALLOWFADEOUT :
			FMOD_STUDIO_STOP_IMMEDIATE;
		event->stop(mode);
	}
}

void SoundEvent::SetPaused(bool pause)
{
	//オーディオシステムのインスタンスがなかったら終了(mSysmteとeventInstanceの両方でnullチェックを行っている)
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
	if (event)
	{
		event->setPaused(pause);
	}
}

void SoundEvent::SetVolume(float value)
{
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
	if (event)
	{
		event->setVolume(value);
	}
}

void SoundEvent::SetPitch(float value)
{
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
	if (event)
	{
		event->setPitch(value);
	}
}

void SoundEvent::SetParameter(const std::string& name, float value)
{
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
	if (event)
	{
		event->setParameterValue(name.c_str(), value);
	}
}

bool SoundEvent::GetPaused() const
{
	bool retVal = false;
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
	if (event)
	{
		event->getPaused(&retVal);
	}
	return retVal;
}

float SoundEvent::GetVolume() const
{
	float retVal = 0.0f;
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
	if (event)
	{
		event->getVolume(&retVal);
	}
	return retVal;
}

float SoundEvent::GetPitch() const
{
	float retVal = 0.0f;
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
	if (event)
	{
		event->getPitch(&retVal);
	}
	return retVal;
}

float SoundEvent::GetParameter(const std::string& name)
{
	float retVal = 0.0f;
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
	if (event)
	{
		event->getParameterValue(name.c_str(), &retVal);
	}
	return retVal;
}

bool SoundEvent::Is3D() const
{
	//bool retVal = false;
	//auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
	//if (event)
	//{
	//	// Get the event description
	//	FMOD::Studio::EventDescription* ed = nullptr;
	//	event->getDescription(&ed);
	//	if (ed)
	//	{
	//		ed->is3D(&retVal);
	//	}
	//}
	//return retVal;
	bool retVal = false;
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
	if(event)
	{
		//イベント記述子を取得
		FMOD::Studio::EventDescription* ed = nullptr;
		//eventに対応するdescriptionの取得
		event->getDescription(&ed);
		if (ed)
		{
			//イベントが2D なのか 3Dなのかを取得
			ed->is3D(&retVal);
		}
	}
	return retVal;
}

namespace
{
	//FMOD_VECTOR VecToFMOD(const Vector3& in)
	//{
	//	// Convert from our coordinates (+x forward, +y right, +z up)
	//	// to FMOD (+z forward, +x right, +y up)
	//	FMOD_VECTOR v;
	//	v.x = in.y;
	//	v.y = in.z;
	//	v.z = in.x;
	//	return v;
	//}

	//FMOD_VECTOR: FMODのベクトル型
	FMOD_VECTOR VecToFMOD(const Vector3& in)
	{
		//このゲームでは左手座標系(+xは前方, +yが右, +zが上)
		//FMODベクトルでは右手座標系(+zが前方, +xが右, +yが上)
		FMOD_VECTOR v;
		v.x = in.y;
		v.y = in.z;
		v.z = in.x;
		return v;
	}
}

void SoundEvent::Set3DAttributes(const Matrix4& worldTrans, Vector3 velocity)
{
	//auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
	//if (event)
	//{
	//	FMOD_3D_ATTRIBUTES attr;
	//	// Set position, forward, up
	//	attr.position = VecToFMOD(worldTrans.GetTranslation());
	//	// In world transform, first row is forward
	//	attr.forward = VecToFMOD(worldTrans.GetXAxis());
	//	// Third row is up
	//	attr.up = VecToFMOD(worldTrans.GetZAxis());
	//	// Set velocity to zero (fix if using Doppler effect)
	//	attr.velocity = { 0.0f, 0.0f, 0.0f };
	//	event->set3DAttributes(&attr);
	//}

	//イベントの位置や方向の更新の際に、この関数にアクターのワールド行列(今回でいうカメラ)を入れるだけでよい
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
	if (event)
	{
		FMOD_3D_ATTRIBUTES attr;
		//位置のセット
		attr.position = VecToFMOD(worldTrans.GetTranslation());
		//前方ベクトルの取得(ワールド行列の第１行)
		attr.forward = VecToFMOD(worldTrans.GetXAxis());
		//上向きベクトルの取得(ワールド行列の代３行)
		attr.up = VecToFMOD(worldTrans.GetZAxis());
		attr.velocity = VecToFMOD(velocity);
		event->set3DAttributes(&attr);
	}
}
