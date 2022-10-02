#pragma once
#define WIN32_LEAN_AND_MEAN

#include <BlueBlur.h>
#include "INIReader.h"
#include "ModLoader.h"
#include "StringHelper.h"
#include "Helpers.h"
#include "Common.h"
#include "ScoreGenerationsAPI.h"
class CSDCommon
{

public:

	static std::vector<Chao::CSD::CScene*> scenesPlayingBack;
	static void PlayAnimation(Chao::CSD::CScene* pScene, const char* name, Chao::CSD::EMotionRepeatType repeatType, float motionSpeed, float startFrame, float endFrame = 0.0f, bool reEnable = false, bool reverse = false)
	{
		if (!pScene) return;
		if (reEnable)
			pScene->SetHideFlag(false);
		
		pScene->SetMotion(name);
		pScene->SetMotionFrame(startFrame);
		float speed = 1;
		if (startFrame == endFrame)
			endFrame = pScene->m_MotionEndFrame;
		if (reverse)
		{
			float temp = startFrame;
			startFrame = endFrame;
			endFrame = startFrame;
			speed = -speed;
			scenesPlayingBack.push_back(pScene);
		}
		if (endFrame > startFrame)
		{
			pScene->m_MotionEndFrame = endFrame;
		}
		pScene->m_MotionDisableFlag = false;
		pScene->m_MotionSpeed = speed;
		pScene->m_MotionRepeatType = repeatType;
		pScene->Update();
	}

	static void FreezeMotion(Chao::CSD::CScene* pScene, float frame = -1)
	{
		if(frame != -1)
		pScene->SetMotionFrame(frame);
		else
		pScene->SetMotionFrame(pScene->m_MotionEndFrame);
		pScene->m_MotionSpeed = 0.0f;
		pScene->m_MotionRepeatType = Chao::CSD::eMotionRepeatType_PlayOnce;
	}
	static void StopMotion(Chao::CSD::CScene* pScene, bool end)
	{
		if(end)
		pScene->SetMotionFrame(pScene->m_MotionEndFrame);		
		pScene->m_MotionSpeed = 0.0f;
		pScene->m_MotionDisableFlag = 1;
		pScene->m_MotionRepeatType = Chao::CSD::eMotionRepeatType_PlayOnce;
	}

	static void IntroAnim(Chao::CSD::RCPtr<Chao::CSD::CScene> scene)
	{
		scene->SetMotion("Intro_Anim");
		scene->SetMotionFrame(0.0f);
		scene->m_MotionSpeed = 1;
		scene->m_MotionRepeatType = Chao::CSD::eMotionRepeatType_PlayOnce;
		scene->Update(0.0f);
	}

	static float lerp(float a, float b, float f)
	{
		return (a * (1.0 - f)) + (b * f);
	}


	static void Initialize();
};

