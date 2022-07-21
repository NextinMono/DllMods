//CSD Object
boost::shared_ptr<Sonic::CGameObjectCSD> spGameplayColors;
//Projects
Chao::CSD::RCPtr<Chao::CSD::CProject> rcGameplayColors;

//Scenes of projects (ideally keep them all separate from each project, unless they're used together)
Chao::CSD::RCPtr<Chao::CSD::CScene> rcRingCount, rcBoostBar;

bool isMission;
//Parameters
size_t prevRingCount;
size_t itemCountDenominator;
bool boostTransitionPlayed = true;
float HudSonicStage::xAspectOffset = 0.0f;
float HudSonicStage::yAspectOffset = 0.0f;

#pragma region XNCPStuff
void HudSonicStage::CreateScreen(Sonic::CGameObject* pParentGameObject)
{
	if (rcGameplayColors && !spGameplayColors)
		pParentGameObject->m_pMember->m_pGameDocument->AddGameObject(spGameplayColors = boost::make_shared<Sonic::CGameObjectCSD>(rcGameplayColors, 0.5f, "HUD", false), "main", pParentGameObject);
}
void HudSonicStage::KillScreen()
{
	if (spGameplayColors)
	{
		spGameplayColors->SendMessage(spGameplayColors->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
		spGameplayColors = nullptr;
	}
}

void HudSonicStage::HudSonicStage::ToggleScreen(const bool visible, Sonic::CGameObject* pParentGameObject)
{
	if (visible)
	{
		HudSonicStage::CreateScreen(pParentGameObject);
	}
	else
		HudSonicStage::KillScreen();
}
void HudSonicStage_IntroFirstFrame(Chao::CSD::RCPtr<Chao::CSD::CScene> scene)
{
	scene->SetMotion("Intro_Anim");
	scene->SetMotionFrame(0);
	scene->m_MotionDisableFlag = 0;
	scene->m_MotionSpeed = 0;
	scene->Update(0);
}
void GetTime(Sonic::CGameDocument* pGameDocument, size_t* minutes, size_t* seconds, size_t* milliseconds)
{
	static uint32_t pAddr = 0xD61570;
	__asm
	{
		mov ecx, minutes
		mov edi, seconds
		mov esi, milliseconds
		mov eax, pGameDocument
		call[pAddr]
	}
}



//REMEMBER TO CALL NULLPTR HERE!!!!
void __fastcall CHudSonicStageRemoveCallback(Sonic::CGameObject* This, void*, Sonic::CGameDocument* pGameDocument)
{
	HudSonicStage::KillScreen();
	Chao::CSD::CProject::DestroyScene(rcGameplayColors.Get(), rcRingCount);
	Chao::CSD::CProject::DestroyScene(rcGameplayColors.Get(), rcBoostBar);
	rcGameplayColors = nullptr;
}


HOOK(void, __fastcall, ProcMsgGetMissionCondition, 0xD0F130, Sonic::CGameObject* This, void* Edx, hh::fnd::Message& in_rMsg)
{
	originalProcMsgGetMissionCondition(This, Edx, in_rMsg);
	itemCountDenominator = *(size_t*)((char*)&in_rMsg + 20);
}

HOOK(void, __fastcall, CHudSonicStageDelayProcessImp, 0x109A8D0, Sonic::CGameObject* This)
{
	ScoreGenerationsAPI::SetVisibility(false);
	originalCHudSonicStageDelayProcessImp(This);
	CHudSonicStageRemoveCallback(This, nullptr, nullptr);


	Sonic::CCsdDatabaseWrapper wrapper(This->m_pMember->m_pGameDocument->m_pMember->m_spDatabase.get());
	HudSonicStage::CalculateAspectOffsets();
	auto spCsdProject = wrapper.GetCsdProject("GAME_INTERFACE");
	rcGameplayColors = spCsdProject->m_rcProject;
	Configuration::Read();
	isMission = !Common::IsCurrentStageBossBattle() && (Common::GetCurrentStageID() & (SMT_Mission1 | SMT_Mission2 | SMT_Mission3 | SMT_Mission4 | SMT_Mission5));

	size_t& flags = ((size_t*)This)[151];
	rcBoostBar = rcGameplayColors->CreateScene("Scene_Wgauge_gauge");/*
	rcBoostBar->SetPosition(HudSonicStage::xAspectOffset, HudSonicStage::yAspectOffset);*/
	rcBoostBar->SetHideFlag(false);
	rcRingCount = rcGameplayColors->CreateScene("Scene_Ringcounter-20");
	rcRingCount->SetPosition(HudSonicStage::xAspectOffset, HudSonicStage::yAspectOffset);
	flags &= ~(0x1 | 0x2 | 0x4 | 0x200 | 0x800); // Mask to prevent crash when game tries accessing the elements we disabled later on


	//if (ScoreGenerationsAPI::IsAttached() && !ScoreGenerationsAPI::IsStageForbidden()) // Score
	//{
	//	rcScoreCount = rcGameplayColors->CreateScene("info_score");
	//}
	//SetAllToIntroFirst();
	HudSonicStage::CreateScreen(This);
}

HOOK(void, __fastcall, CHudSonicStageUpdateParallel, 0x1098A50, Sonic::CGameObject* This, void* Edx, const hh::fnd::SUpdateInfo& in_rUpdateInfo)
{
	originalCHudSonicStageUpdateParallel(This, Edx, in_rUpdateInfo);
	HudSonicStage::ToggleScreen(*(bool*)0x1A430D8, This); // ms_IsRenderGameMainHud

	if (!spGameplayColors)
		return;


	char text[256];
	size_t rowIndex = 1;
	const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
	//if (rcRingCount && playerContext)
	//{
	//	sprintf(text, "%04d", playerContext->m_RingCount);
	//	rcRingCount->GetNode("num_ring")->SetText(text);

	//	if (prevRingCount < playerContext->m_RingCount)
	//	{
	//		CSDCommon::PlayAnimation(*rcRingCount, "get_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	//	}

	//	prevRingCount = playerContext->m_RingCount;
	//}
}
HOOK(void, __fastcall, MsgChangeCustomHud, 0x10947A0, Sonic::CGameObject* This, void* Edx, hh::fnd::Message& in_rMsg)
{
	const size_t iconType = *(size_t*)((char*)&in_rMsg + 16);
	const size_t iconTypeT = *(size_t*)((char*)&in_rMsg + 8);
	const size_t spriteCount = *(size_t*)((char*)&in_rMsg + 20);
	/*WispSet(iconType);
	printf("\nCURRENT SKILL: %d", iconType);
	if (spriteCount < 1)
	{
		wispAcquired = false;
		WispBarSet(false);
		skillIndex = -1;
	}
	else
	{
		wispAcquired = true;
		WispBarSet(true);
		skillIndex = iconType;
	}
	skillIndex = iconType;*/

}
/// <summary>
/// Findings:
/// UI_GP_TRICK: 011B37A7, called the same as the xncp, i think this is how it actually controls it
/// 
/// </summary>


HOOK(void, __fastcall, ProcMsgChangeWispHud, 0x1096050, Sonic::CGameObject* This, void* Edx, hh::fnd::Message& in_rMsg)
{
	const size_t motionType = *(size_t*)((char*)&in_rMsg + 16);

	const size_t wispType = *(size_t*)((char*)&in_rMsg + 20);

	/*if (rcWispContainer && !isUsingWisp)
	{
		if (!wispAcquired)
			WispBarSet(true);
		wispAcquired = true;
		if (wispType == 1)
		{
			WispSet(22);
		}
		if (wispType == 0)
		{
			WispSet(21);
		}
	}*/
}
HOOK(void, __stdcall, CPlayerGetLife, 0xE75520, Sonic::Player::CPlayerContext* context, int lifeCount, bool playSound)
{
	/*originalCPlayerGetLife(context, lifeCount, playSound);*/

	//if (lifeCount > 0)
	//{
	//	if (rcLife)
	//	{
	//		rcLife->GetNode("icon")->SetPosition(1, 1);
	//		CSDCommon::PlayAnimation(*rcLife, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, false, false);
	//	}
	//	lifeGoing = true;
	//	lifeAnim = 1;
	//	if (playSound)
	//	{
	//		context->PlaySound(4001009, 0);
	//	}
	//}
}
#pragma endregion
#pragma region HyperBE32 Original Code
// https://github.com/brianuuu/DllMods/blob/master/Source/Sonic06HUD/Stage.cpp#L166
HOOK(void, __fastcall, CHudSonicStageUpdate, 0x1098A50, void* thisDeclaration, void* edx, float* pUpdateInfo)
{
	// Force disable extended boost.
	*(uint32_t*)((uint32_t)*CONTEXT->ms_pInstance + 0x680) = 1;

	// Always clamp boost to 100.
	CONTEXT->m_ChaosEnergy = min(CONTEXT->m_ChaosEnergy, 100.0f);

	originalCHudSonicStageUpdate(thisDeclaration, edx, pUpdateInfo);
}
// https://github.com/brianuuu/DllMods/blob/master/Source/NavigationLightdashSound/NavigationSound.cpp#L14
HOOK(void, __fastcall, MsgStartCommonButtonSign, 0x5289A0, void* thisDeclaration, void* edx, uint32_t a2)
{
	// Disable Y button prompt.
	if (*(uint32_t*)(a2 + 16) == 3)
		return;

	originalMsgStartCommonButtonSign(thisDeclaration, edx, a2);
}
#pragma endregion
//Brianuu/Skyth

/*
	t;*/


void HudSonicStage::Install()
{
	INSTALL_HOOK(CHudSonicStageUpdate);
	//INSTALL_HOOK(MsgStartMode);
	//INSTALL_HOOK(MsgRestartStage);
	/*INSTALL_HOOK(TestRestart);*/

	INSTALL_HOOK(ProcMsgGetMissionCondition);
	INSTALL_HOOK(CHudSonicStageDelayProcessImp);
	INSTALL_HOOK(CHudSonicStageUpdateParallel);
	//INSTALL_HOOK(MsgChangeCustomHud);
	//INSTALL_HOOK(ProcMsgChangeWispHud);
	INSTALL_HOOK(CPlayerGetLife);
	WRITE_MEMORY(0x16A467C, void*, CHudSonicStageRemoveCallback);

	WRITE_MEMORY(0x109B1A4, uint8_t, 0xE9, 0xDC, 0x02, 0x00, 0x00); // Disable lives
	WRITE_MEMORY(0x109B490, uint8_t, 0x90, 0xE9); // Disable time
	WRITE_MEMORY(0x109B5AD, uint8_t, 0x90, 0xE9); // Disable rings
	WRITE_MEMORY(0x109B8F5, uint8_t, 0x90, 0xE9); // Disable boost gauge
	WRITE_MEMORY(0x109BC88, uint8_t, 0x90, 0xE9); // Disable boost button


	INSTALL_HOOK(MsgStartCommonButtonSign);
	// Patch ring counter to use four digits.
	WRITE_MEMORY(0x168D33C, const char, "%04d");
	WRITE_MEMORY(0x168E8E0, const char, "%04d");

	// Disable boost button guide animation.
	if (!Configuration::GaugeShake)
		WRITE_NOP(0x124F4A1, 2);


}

void HudSonicStage::CalculateAspectOffsets()
{
	if (*(size_t*)0x6F23C6 != 0x75D8C0D9) // Widescreen Support
	{
		const float aspect = (float)*(size_t*)0x1DFDDDC / (float)*(size_t*)0x1DFDDE0;

		if (aspect * 9.0f > 16.0f)
		{
			xAspectOffset = 720.0f * aspect - 1280.0f;
			yAspectOffset = 0.0f;
		}
		else
		{
			xAspectOffset = 0.0f;
			yAspectOffset = 1280.0f / aspect - 720.0f;
		}
	}
	else
	{
		xAspectOffset = 0.0f;
		yAspectOffset = 0.0f;
	}
}