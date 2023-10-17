#include "HudLoading.h"
#include "Configuration.h"

Chao::CSD::RCPtr<Chao::CSD::CProject> rcProjectLoading;
Chao::CSD::RCPtr<Chao::CSD::CScene> ogLoad;
char* textTest;
HudLoading::StageData m_stageData;
int rankLoading;
bool m_isBG1Intro = false;
bool m_exitingStage = false;
bool m_exitFadeStarted = false;
bool m_isMissionLoadingText = false;
bool m_isEvent = false;
bool m_loadingDisplayHint = false;
float m_applicationDeltaTime = 0.0f;

void HudLoading_CreateScene(hh::fnd::CStateMachineBase::CStateBase* This)
{
	if (rcProjectLoading) return;

	void* contextBase = This->GetContextBase();
	rcProjectLoading = *(Chao::CSD::RCPtr<Chao::CSD::CProject>*)((uint32_t)contextBase + 188);
	if (!rcProjectLoading) return;
	ogLoad = rcProjectLoading->CreateScene("load_classic");
	ogLoad->SetHideFlag(true);

}
void HudLoading_PlayMotion(Chao::CSD::RCPtr<Chao::CSD::CScene>& scene, char const* motion, float startFrame = 0.0f, bool loop = false)
{
	if (!scene) return;
	scene->SetHideFlag(false);
	scene->SetMotion(motion);
	scene->SetMotionFrame(startFrame);
	scene->m_MotionDisableFlag = false;
	scene->m_MotionSpeed = 1.0f;
	scene->m_MotionRepeatType = loop ? Chao::CSD::eMotionRepeatType_Loop : Chao::CSD::eMotionRepeatType_PlayOnce;
	scene->Update();
}
HOOK(int*, __fastcall, HudLoading_CHudLoadingCStateIdleBegin, 0x1092710, hh::fnd::CStateMachineBase::CStateBase* This)
{
	HudLoading_CreateScene(This);

	return originalHudLoading_CHudLoadingCStateIdleBegin(This);
}
HOOK(void, __fastcall, HudLoading_CHudLoadingCStateIntroBegin, 0x10938F0, hh::fnd::CStateMachineBase::CStateBase* This)
{
	uint8_t stageID = Common::GetCurrentStageID() & 0xFF;
	//White world
	if (stageID == SMT_pam000)
	{
		originalHudLoading_CHudLoadingCStateIntroBegin(This);
		return;
	}
	char const* eventName = *(char**)Common::GetMultiLevelAddress(0x1E66B34, { 0x4, 0x1B4, 0x80, 0x2C });
	m_isEvent = !std::string(eventName).empty();
	HudLoading_CreateScene(This);

	m_loadingDisplayHint = false;
	
	m_isBG1Intro = true;
	ogLoad->GetNode("score")->SetHideFlag(true);
	HudLoading_PlayMotion(ogLoad, "Intro_Anim");
	//TODO: Make this use clamp instead of this awful math
	int stageName = stageID % 2 ? stageID - 1 : stageID;
	stageName-= stageName / 2;
	if (stageName < 0)
		stageName = 0;
	
	ogLoad->GetNode("word_stagename")->SetPatternIndex(stageName);
	if (Configuration::SpriteType == 0)
	{
		//top_shade = 1
		//act2 = 2;
		if (stageID % 2)
		{
			ogLoad->GetNode("gear_1")->SetHideFlag(false);
			ogLoad->GetNode("square")->SetHideFlag(true);
		}
		else
		{
			ogLoad->GetNode("gear_1")->SetHideFlag(true);
			ogLoad->GetNode("square")->SetHideFlag(false);
		}
	}
	if (Configuration::SpriteType == 1)
	{
		//top_shade = 1
		//act2 = 2;
		if (stageID % 2)
		{
			ogLoad->GetNode("top_0")->SetHideFlag(false);
			ogLoad->GetNode("top_shade")->SetHideFlag(true);
		}
		else
		{
			ogLoad->GetNode("top_0")->SetHideFlag(true);
			ogLoad->GetNode("top_shade")->SetHideFlag(false);
		}
	}

	/*if (Common::IsCurrentStageBossBattle()) {
		rcActInfo->GetNode("act_num")->SetPatternIndex(6);
		rcActInfo->GetNode("stage_name")->SetHideFlag(true);
	}*//*
	HudLoading_PlayMotion(rcLoadingBG, "Intro_Anim", 0.0f, false);*/
	/*rcActInfo->SetHideFlag(stageID == SMT_pam000);
	rcLoadingBG->SetHideFlag(stageID == SMT_pam000);*/
	std::srand(static_cast<unsigned int>(std::time(nullptr)));
	int hintPattern = std::rand() % 7;


	originalHudLoading_CHudLoadingCStateIntroBegin(This);
}



HOOK(int32_t*, __fastcall, HudLoading_CHudLoadingCStateIntroAdvance, 0x10936B0, hh::fnd::CStateMachineBase::CStateBase* This)
{
	uint8_t stageID = Common::GetCurrentStageID() & 0xFF;
	

	if (m_isEvent)
	{
		This->m_pStateMachine->ChangeState("Usual");
	}
	

	return nullptr;
}
HOOK(void*, __fastcall, HudLoading_UpdateApplication, 0xE7BED0, void* This, void* Edx, float elapsedTime, uint8_t a3)
{
	m_applicationDeltaTime = elapsedTime;
	return originalHudLoading_UpdateApplication(This, Edx, elapsedTime, a3);
}

HOOK(int, __fastcall, HudLoading_CHudLoadingCStateUsualAdvance, 0x10926E0, hh::fnd::CStateMachineBase::CStateBase* This)
{
	uint8_t stageID = Common::GetCurrentStageID() & 0xFF;

	return originalHudLoading_CHudLoadingCStateUsualAdvance(This);
}

HOOK(int32_t*, __fastcall, HudLoading_CHudLoadingCStateOutroBegin, 0x1093410, hh::fnd::CStateMachineBase::CStateBase* This)
{
	if (!m_isEvent)
	{
		m_isBG1Intro = false;
		HudLoading_PlayMotion(ogLoad, "Outro_Anim");

		uint8_t stageID = Common::GetCurrentStageID() & 0xFF;
	}

	return originalHudLoading_CHudLoadingCStateOutroBegin(This);
}

HOOK(int32_t*, __fastcall, HudLoading_CHudLoadingCStateOutroAdvance, 0x1092EE0, hh::fnd::CStateMachineBase::CStateBase* This)
{
	/*if (m_isEvent && This->m_Time > 0.24f)
	{
		rcLoadingEvent->SetHideFlag(true);
		rcLoadingPDATxt->SetHideFlag(true);
	}*/

	return originalHudLoading_CHudLoadingCStateOutroAdvance(This);
}
void __fastcall CHudSonicLoadingRemoveCallback(Sonic::CGameObject* This, void*, Sonic::CGameDocument* pGameDocument)
{

	Chao::CSD::CProject::DestroyScene(rcProjectLoading.Get(), ogLoad);
	HudSonicStage::KillScreen();

	rcProjectLoading = nullptr;
}
void HudLoading::Install()
{
	WRITE_MEMORY(0x16A467C, void*, CHudSonicLoadingRemoveCallback);
	switch (Configuration::SpriteType)
	{
	case 0 :
	{
		WRITE_MEMORY(0x0168CC04, char, "ui_loading");
		WRITE_MEMORY(0x015B3C7C, char, "ui_loading");
		break;
	
	}
	case 1:
	{
		WRITE_MEMORY(0x0168CC04, char, "s2_loading");
		WRITE_MEMORY(0x015B3C7C, char, "s2_loading");
		break;
	}
	}
	// Disable loading screen sfx
	WRITE_MEMORY(0x44A2E8, int, -1);
	WRITE_MEMORY(0x44A4F5, int, -1);
	WRITE_MEMORY(0x10933C1, int, -1);
	WRITE_MEMORY(0x1093651, int, -1);
	WRITE_MEMORY(0x1093562, int, -1);
	WRITE_MEMORY(0x10936EC, int, -1);
	// State hooks
	INSTALL_HOOK(HudLoading_CHudLoadingCStateIntroBegin);
	INSTALL_HOOK(HudLoading_CHudLoadingCStateIntroAdvance);
	INSTALL_HOOK(HudLoading_CHudLoadingCStateUsualAdvance);
	INSTALL_HOOK(HudLoading_CHudLoadingCStateOutroBegin);
	INSTALL_HOOK(HudLoading_CHudLoadingCStateOutroAdvance);

	// Transition out to loading screen
	INSTALL_HOOK(HudLoading_UpdateApplication);

}