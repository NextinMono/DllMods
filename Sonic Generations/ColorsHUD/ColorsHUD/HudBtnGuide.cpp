#include <inttypes.h>
//for BtnGuide, I mean the HUD's BtnGuide. Although there's some stuff for the general BtnGuide
//Thanks SEGA for the naming!
Sonic::Player::CPlayerSpeedContext::EStateFlag state;
Chao::CSD::RCPtr<Chao::CSD::CProject> rcBtnGuideColors;
Chao::CSD::RCPtr<Chao::CSD::CScene> trick_text, onebtn, sign;
boost::shared_ptr<Sonic::CGameObjectCSD> spBtnGuideColors;
HudBtnGuide::BtnGuideState SignState;
bool moved = false;
bool tricksStarted = false;
bool introAnimPlayed = false;
bool rightAfter = false;
int textChoice = -1;
float timeBetweenAnim = 0;
bool endingQuickstep;
int lastNavigationType = -1;
inline FUNCTION_PTR(void, __fastcall, AdvanceTrickss, 0x52F390, Sonic::CGameObject* This, void* Edx, int a2);
inline FUNCTION_PTR(void, __fastcall, FinalTricks, 0x52F8F0, Sonic::CGameObject* This, void* Edx, int a2);
bool isSlowedDown = false;




int maxTricks = 4;
int currentTricks = 0;
float timeSinceStartTricks = 0;
bool areTricksComplete;
bool tricksLocked = false;
bool* GetTimeScaleEnabled()
{
	return (bool*)Common::GetMultiLevelAddress(0x1E0BE5C, { 0x8, 0x19C });
}

float* GetTimeScale()
{
	return (float*)Common::GetMultiLevelAddress(0x1E0BE5C, { 0x8, 0x1A0 });
}

void SlowTime(float dt)
{
	isSlowedDown = true;
	*(bool*)Common::GetMultiLevelAddress(0x1E0BE5C, { 0x8, 0x19C }) = true;
	*GetTimeScale() = max(*GetTimeScale() - dt * 0.2f, 0.08f);
}

void ResetTime()
{
	if (*GetTimeScaleEnabled())
	{
		isSlowedDown = false;
		*GetTimeScaleEnabled() = false;
		*GetTimeScale() = 1.0f;
	}
}
void HudBtnGuide::CreateScreen(Sonic::CGameObject* pParentGameObject)
{

	if (rcBtnGuideColors && !spBtnGuideColors)
		pParentGameObject->m_pMember->m_pGameDocument->AddGameObject(spBtnGuideColors = boost::make_shared<Sonic::CGameObjectCSD>(rcBtnGuideColors, 0.5f, "HUD", false), "main", pParentGameObject);
}
void HudBtnGuide::KillScreen()
{
	if (spBtnGuideColors)
	{
		spBtnGuideColors->SendMessage(spBtnGuideColors->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
		spBtnGuideColors = nullptr;
	}
}

void HudBtnGuide::ToggleScreen(const bool visible, Sonic::CGameObject* pParentGameObject)
{
	if (visible)
	{
		HudBtnGuide::CreateScreen(pParentGameObject);
	}
	else
		HudBtnGuide::KillScreen();
}
void __fastcall HGT_CHudSonicStageRemoveCallback(Sonic::CGameObject* This, void*, Sonic::CGameDocument* pGameDocument)
{
	HudBtnGuide::KillScreen();

	Chao::CSD::CProject::DestroyScene(rcBtnGuideColors.Get(), trick_text);
	Chao::CSD::CProject::DestroyScene(rcBtnGuideColors.Get(), onebtn);
	Chao::CSD::CProject::DestroyScene(rcBtnGuideColors.Get(), sign);
	rcBtnGuideColors = nullptr;
	tricksStarted, introAnimPlayed = false;
	textChoice = -1;

}

HOOK(void, __fastcall, HGT_CHudSonicStageDelayProcessImp, 0x109A8D0, Sonic::CGameObject* This)
{
	originalHGT_CHudSonicStageDelayProcessImp(This);

	HGT_CHudSonicStageRemoveCallback(This, nullptr, nullptr);
	Sonic::CCsdDatabaseWrapper wrapper(This->m_pMember->m_pGameDocument->m_pMember->m_spDatabase.get());

	auto spCsdProject = wrapper.GetCsdProject("ui_gameplay_btn_guide");
	rcBtnGuideColors = spCsdProject->m_rcProject;
	trick_text = rcBtnGuideColors->CreateScene("trickjump");
	onebtn = rcBtnGuideColors->CreateScene("1btn");
	sign = rcBtnGuideColors->CreateScene("sign");

	trick_text->GetNode("text")->SetPatternIndex(0);
	trick_text->GetNode("text_0001")->SetPatternIndex(0);
	CSDCommon::PlayAnimation(*trick_text, "Outro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	trick_text->SetHideFlag(true);

	onebtn->GetNode("icon_btn")->SetPatternIndex(11);
	onebtn->GetNode("icon_btn_KB_text")->SetHideFlag(true);
	CSDCommon::PlayAnimation(*onebtn, "Outro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	onebtn->SetHideFlag(true);

	CSDCommon::PlayAnimation(*sign, "Intro_Anim_df_r", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	sign->SetHideFlag(true);

	HudBtnGuide::CreateScreen(This);
}

void DisplayAdvancedTrickText(int specific)
{
	trick_text->GetNode("text")->SetPatternIndex(specific);
	trick_text->GetNode("text_0001")->SetPatternIndex(specific);
	trick_text->m_MotionDisableFlag = 0;
	trick_text->SetHideFlag(false);
	CSDCommon::PlayAnimation(*trick_text, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	introAnimPlayed = true;

}

HOOK(void, __fastcall, AdvanceTricks, 0x52F390, Sonic::CGameObject* This, void* edx, int a2)
{
	return originalAdvanceTricks(This, edx, a2);
}

HOOK(void, __fastcall, HGT_CHudSonicStageUpdateParallel, 0x1098A50, Sonic::CGameObject* This, void* Edx, const hh::fnd::SUpdateInfo& in_rUpdateInfo)
{
	Configuration::Read();
	switch (SignState)
	{
	case HudBtnGuide::None:
	{
		break;
	}
	case HudBtnGuide::Drift:
	{
		if (sign->m_MotionDisableFlag == 1)
		{
			CSDCommon::PlayAnimation(*sign, "Usual_Anim_df", Chao::CSD::eMotionRepeatType_Loop, 1, 0);
		}
		break;
	}
	case HudBtnGuide::Quickstep:
	{
		if (sign->m_MotionDisableFlag == 1)
		{
			CSDCommon::PlayAnimation(*sign, "Usual_Anim_qs", Chao::CSD::eMotionRepeatType_Loop, 1, 0);
		}
		break;
	}
	}
	const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
	if (introAnimPlayed)
	{
		if (trick_text->m_MotionDisableFlag == 1)
		{
			CSDCommon::PlayAnimation(*trick_text, "Outro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);

			introAnimPlayed = false;
		}

	}
	if (textChoice <= 0 && tricksStarted && onebtn->m_MotionDisableFlag == 1)
	{
		CSDCommon::PlayAnimation(*onebtn, "pelter_Anim", Chao::CSD::eMotionRepeatType_Loop, 1, 0);
	}
}

HOOK(void, __fastcall, Test, 0x11B3790, Sonic::CGameObject* This, void* Edx, int a2, int a3, int* a4)
{
	return originalTest(This, Edx, a2, a3, a4);
}
HOOK(int, _cdecl, TestTwo, 0x12258B0, int a1)
{
	return originalTestTwo(a1);
}
HOOK(void, *_fastcall, NavigationCollisionSignal, 0x1222CC0, int This, void* Edx, int a2)
{
	int navigationType = *(DWORD*)(This + 348);
	uint8_t stageID = Common::GetCurrentStageID() & 0xFF;
	if (!stageID % 2 || rightAfter)
	{
		rightAfter = false;
		return originalNavigationCollisionSignal(This, Edx, a2);
	}
	//Check if any message enabled other stuff first

	////Toggle
	//if (lastNavigationType == navigationType && lastNavigationType != -1) //Hide
	//{
	//	CSDCommon::PlayAnimation(*sign, "Outro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	//	SignState = HudBtnGuide::None;
	//	lastNavigationType = -1;
	//}
	//if (SignState != HudBtnGuide::None && lastNavigationType == -1)
	//{
	//	switch (SignState)
	//	{

	//	}
	//	lastNavigationType = 8;
	//}
	//else //Show
	//{


	if (sign->m_MotionDisableFlag)
	{
		switch (SignState)
		{
		case HudBtnGuide::Quickstep:
		{
			//quickstep
			SignState = HudBtnGuide::None;
			sign->SetHideFlag(false);
			CSDCommon::PlayAnimation(*sign, "Outro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
			lastNavigationType = 0;
			break;
		}
		}

		//case 6:
		//{
		//	//Button to Button
		//	break;
		//}
		//case 7:
		//case 8:
		//{

		//	SignState = HudBtnGuide::Drift;
		//	sign->SetHideFlag(false);
		//	CSDCommon::PlayAnimation(*sign, "Intro_Anim_df_r", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		//	lastNavigationType = 8;
		//	break;
		//}
	}
	//}

	//int navigationType = *(DWORD*)(This + 348);
	//uint8_t stageID = Common::GetCurrentStageID() & 0xFF;
	//if (!stageID % 2)
	//{
	//	return originalNavigationCollisionSignal(This, Edx, a2);
	//}
	////Check if any message enabled other stuff first

	////Toggle
	//if (lastNavigationType == navigationType && lastNavigationType != -1) //Hide
	//{
	//	CSDCommon::PlayAnimation(*sign, "Outro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	//	SignState = HudBtnGuide::None;
	//	lastNavigationType = -1;
	//}
	//if (SignState != HudBtnGuide::None && lastNavigationType == -1)
	//{
	//	switch (SignState)
	//	{

	//	}
	//	lastNavigationType = 8;
	//}
	//else //Show
	//{
	//	switch (navigationType)
	//	{
	//	case 0:
	//	{
	//		//quickstep
	//		SignState = HudBtnGuide::Quickstep;
	//		sign->SetHideFlag(false);

	//		CSDCommon::PlayAnimation(*sign, "Intro_Anim_qs", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	//		lastNavigationType = 0;
	//		break;
	//	}
	//	case 6:
	//	{
	//		//Button to Button
	//		break;
	//	}
	//	case 7:
	//	case 8:
	//	{

	//		SignState = HudBtnGuide::Drift;
	//		sign->SetHideFlag(false);
	//		CSDCommon::PlayAnimation(*sign, "Intro_Anim_df_r", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	//		lastNavigationType = 8;
	//		break;
	//	}
	//	}
	//}


	return originalNavigationCollisionSignal(This, Edx, a2);
}
HOOK(int, __fastcall, TestTh, 0x1094820, Sonic::CGameObject* This, void* Edx, int a2)
{
	return originalTestTh(This, Edx, a2);
}

HOOK(int, __stdcall, sub_B21A30, 0xB21A30, Sonic::CGameObject* a1)
{
	return originalsub_B21A30(a1);
}

HOOK(int, __stdcall, Test8, 0xD6B310, int a1, int a2, int a3, Hedgehog::Base::CSharedString* a4, void* Edx)
{
	printf("TRIG");
	return originalTest8(a1, a2, a3, a4, Edx);
}
HOOK(int, __cdecl, MsgStartQuickStepSign, 0x46F250, int a1, char* a2, hh::math::CVector* a3, int* a4)
{
	SignState = HudBtnGuide::Quickstep;
	sign->SetHideFlag(false);

	CSDCommon::PlayAnimation(*sign, "Intro_Anim_qs", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	lastNavigationType = 0;

	rightAfter = true;
	return originalMsgStartQuickStepSign(a1, a2, a3, a4);
}

HOOK(void, __stdcall, MsgStartLeftRightSign, 0xF09480, int a1)
{
	SignState = HudBtnGuide::Quickstep;
	sign->SetHideFlag(false);

	CSDCommon::PlayAnimation(*sign, "Intro_Anim_qs", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	lastNavigationType = 0;
	originalMsgStartLeftRightSign(a1);
}
HOOK(void, __fastcall, MsgEndQuickStepSign, 0x10F9E80, DWORD* This, int a2, void* Edx)
{
	return originalMsgEndQuickStepSign(This, a2, Edx);
}
HOOK(char, *__fastcall, sub_E41840, 0xE41840, void* This, void* Edx) {
	return originalsub_E41840(This, Edx);
}
HOOK(DWORD, *__cdecl, sub_525B70, 0x525B70, DWORD* This, void* Edx)
{
	printf("stuff");
	return originalsub_525B70(This, Edx);
}
float initialYVelocity = 10;
bool applyYVelocity = false;
bool customVelocity = true;
inline FUNCTION_PTR(void, __thiscall, AdvTest, 0x52F390, Sonic::CGameObject* This, int a2);
HOOK(void, __fastcall, StartTricks, 0x52F030, Sonic::CGameObject* This, void* edx, int a2)
{
	areTricksComplete = false;
	tricksLocked = false;
	originalStartTricks(This, edx, a2);
	textChoice = -1;
	tricksStarted = true;
	trick_text->SetHideFlag(false);
	onebtn->SetHideFlag(false);

	if (customVelocity)
	{
		//If the trick ramp isn't vertical, divide by 2
		if (CONTEXT->m_Velocity.y() < abs(CONTEXT->m_Velocity.x()) || CONTEXT->m_Velocity.y() < abs(CONTEXT->m_Velocity.z()))
		{
			CONTEXT->m_Velocity.x() /= 2;
			CONTEXT->m_Velocity.z() /= 2;
			applyYVelocity = true;
		}
		else
			applyYVelocity = false;
	}
	//initialYVelocity = CONTEXT->m_Velocity.y();
	CSDCommon::PlayAnimation(*onebtn, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
}
HOOK(void, __fastcall, TrickUpdate, 0xE4B3F0, DWORD* This)
{
	char debug[32];
	snprintf(debug, 32, "Vel: %f, %f, %f", CONTEXT->m_Velocity.x(), CONTEXT->m_Velocity.y(), CONTEXT->m_Velocity.z());
	DebugDrawText::log(debug, 0);
	auto inputState = Sonic::CInputState::GetInstance();
	auto inputPtr = &inputState->m_PadStates[inputState->m_CurrentPadStateIndex];
	timeSinceStartTricks++;
	if (!areTricksComplete)
	{
		if (!tricksLocked)
		{
			if (inputPtr->IsTapped(Sonic::eKeyState_A) && timeSinceStartTricks > 10)
			{
				ResetTime();
				timeSinceStartTricks = 0;
				currentTricks++;
				if (currentTricks > maxTricks)
				{
					*((byte*)This + 120) = 1;
					currentTricks = 0;
					areTricksComplete = true;
					CONTEXT->ChangeState("Fall");
					if (applyYVelocity)
					{
						CONTEXT->m_Velocity.x() *= 2;
						CONTEXT->m_Velocity.z() *= 2;
					}
				}
				if(applyYVelocity && customVelocity)
				CONTEXT->m_Velocity.y() = initialYVelocity;
				textChoice = currentTricks;
				This[24] = 1; //State
				This[32] = currentTricks;
				This[33] = currentTricks;
				This[34] = currentTricks; //trick amount?
				originalTrickUpdate(This);
				DisplayAdvancedTrickText(currentTricks - 1);
			}
			if (timeSinceStartTricks > 5 && currentTricks == 0)
			{
				SlowTime(0.1f);
			}
			else if (timeSinceStartTricks > 30 && currentTricks > 0)
			{
				SlowTime(0.1f);
			}
		}

	}
	else
	{
		//Exit out of everything related to tricks
		if (!tricksLocked)
		{
			areTricksComplete = false;
			tricksLocked = true;
			This[24] = 3;
			originalTrickUpdate(This);
			return;
		}
	}


	char buff[16];
	snprintf(buff, 16, "State: %x", This[24]);
	DebugDrawText::log(buff, 0);/*
	originalTrickUpdate(This);*/
}
HOOK(void, __fastcall, FinalTrick, 0x52F8F0, Sonic::CGameObject* This, void* Edx, int a2)
{
	const byte mode = *(byte*)(a2 + 16);
	tricksStarted = false;
	if (mode != 0)
	{
		trick_text->GetNode("text")->SetPatternIndex(4);
		trick_text->GetNode("text_0001")->SetPatternIndex(4);
		CSDCommon::PlayAnimation(*trick_text, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		introAnimPlayed = true;
	}
	currentTricks = 0;
	CSDCommon::PlayAnimation(*onebtn, "Outro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	textChoice = -1;
	return originalFinalTrick(This, Edx, a2);
}
HOOK(Hedgehog::Universe::MessageTypeSet, *__fastcall, SendMessage, 0x76AA80, Hedgehog::Universe::MessageTypeSet* This, void* Edx)
{

	printf("\n%d\n", This->m_SenderActorID);
	return originalSendMessage(This, Edx);
}
//void __declspec(naked) TestAButton()
//{
//	static uint32_t returnAddress = 0x0E4B53C;
//	__asm
//	{		
//		jmp[returnAddress]
//	}
//}
//00E4B4FE
void __declspec(naked) ForceTricks()
{
	static uint32_t sub_446E30 = 0x446E30;
	static uint32_t returnAddress = 0x01097130;
	static int skill;
	__asm
	{
		jmp[returnAddress]
	}
}
void HudBtnGuide::Install()
{
	//INSTALL_HOOK(MsgStartQuickStepSign);/*
	//INSTALL_HOOK(TestSetTricksState);*/
	//INSTALL_HOOK(sub_525B70);
	//INSTALL_HOOK(sub_E41840);
	//INSTALL_HOOK(MsgEndQuickStepSign);
	//INSTALL_HOOK(MsgStartLeftRightSign);
	//INSTALL_HOOK(sub_B21A30);
	//INSTALL_HOOK(TestTh);
	//INSTALL_HOOK(TestTwo);
	//INSTALL_HOOK(NavigationCollisionSignal);
	//INSTALL_HOOK(Test);
	/*WRITE_JUMP(0x00E4B625, 0x00E4B632);*/
	/*WRITE_JUMP(0x00E4B4E4, 0x00E4B552);*/

	//Override stick input (?)
	WRITE_JUMP(0x00E4B556, 0x00E4B561);

	WRITE_JUMP(0x00E4BC5A, 0x00E4BC67);

	//// Always go lower path initially
	//WRITE_MEMORY(0x1014870, uint8_t, 0xE9, 0x1E, 0x01, 0x00, 0x00);

	INSTALL_HOOK(StartTricks);
	INSTALL_HOOK(TrickUpdate);
	INSTALL_HOOK(FinalTrick);
	INSTALL_HOOK(AdvanceTricks);
	INSTALL_HOOK(HGT_CHudSonicStageUpdateParallel);
	INSTALL_HOOK(HGT_CHudSonicStageDelayProcessImp);

}