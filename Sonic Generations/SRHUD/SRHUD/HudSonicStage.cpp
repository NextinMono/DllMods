//CSD Object
boost::shared_ptr<Sonic::CGameObjectCSD> spGameInterface;
//Projects
Chao::CSD::RCPtr<Chao::CSD::CProject> rcGameInterface;

//Scenes of projects (ideally keep them all separate from each project, unless they're used together)
Chao::CSD::RCPtr<Chao::CSD::CScene> lifeCount, ringCount, soulGauge, fullEffect, fullEffect2, loseRing, base, fullMask;

bool isMission;
//Parameters
size_t prevRingCount;
size_t itemCountDenominator;
bool boostFull = false;
float HudSonicStage::xAspectOffset = 0.0f;
float HudSonicStage::yAspectOffset = 0.0f;
int ringIndex;
bool spinningRing;
int repeatTime;
bool isClassic = false;
#pragma region XNCPStuff
void HudSonicStage::CreateScreen(Sonic::CGameObject* pParentGameObject)
{
	if (rcGameInterface && !spGameInterface)
		pParentGameObject->m_pMember->m_pGameDocument->AddGameObject(spGameInterface = boost::make_shared<Sonic::CGameObjectCSD>(rcGameInterface, 0.5f, "HUD", false), "main", pParentGameObject);
}
void HudSonicStage::KillScreen()
{
	if (spGameInterface)
	{
		spGameInterface->SendMessage(spGameInterface->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
		spGameInterface = nullptr;
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
	Chao::CSD::CProject::DestroyScene(rcGameInterface.Get(), ringCount);
	Chao::CSD::CProject::DestroyScene(rcGameInterface.Get(), soulGauge);
	Chao::CSD::CProject::DestroyScene(rcGameInterface.Get(), fullEffect);
	Chao::CSD::CProject::DestroyScene(rcGameInterface.Get(), fullEffect2);
	Chao::CSD::CProject::DestroyScene(rcGameInterface.Get(), lifeCount);
	Chao::CSD::CProject::DestroyScene(rcGameInterface.Get(), loseRing);
	Chao::CSD::CProject::DestroyScene(rcGameInterface.Get(), base);
	Chao::CSD::CProject::DestroyScene(rcGameInterface.Get(), fullMask);
	rcGameInterface = nullptr;
	lifeCount, ringCount, soulGauge, fullEffect, fullEffect2, loseRing, base, fullMask = nullptr;
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


	Configuration::Read();
	Sonic::CCsdDatabaseWrapper wrapper(This->m_pMember->m_pGameDocument->m_pMember->m_spDatabase.get());
	HudSonicStage::CalculateAspectOffsets();
	std::string xncpname = "";
	if (Configuration::IsBeta)
		xncpname = "GAME_INTERFACE_E3";
	else
		xncpname = "GAME_INTERFACE";
	auto spCsdProject = wrapper.GetCsdProject(xncpname.c_str());
	rcGameInterface = spCsdProject->m_rcProject;
	isMission = !Common::IsCurrentStageBossBattle() && (Common::GetCurrentStageID() & (SMT_Mission1 | SMT_Mission2 | SMT_Mission3 | SMT_Mission4 | SMT_Mission5));


	size_t& flags = ((size_t*)This)[151];

	isClassic = !(flags & 0x200);

	if (Configuration::IsBeta)
	{
		soulGauge = rcGameInterface->CreateScene("Scene_Wgauge_gauge");
		ringCount = rcGameInterface->CreateScene("Scene_Ringcounter");
		//rcGameInterface->CreateScene("Scene_Ringcounter-20");
		fullEffect = rcGameInterface->CreateScene("Scene_Wgauge_gaugefull");
		fullEffect2 = rcGameInterface->CreateScene("Scene_Wgauge_gaugefull2");
		fullMask = rcGameInterface->CreateScene("Scene_Wgauge_mask");
		loseRing = rcGameInterface->CreateScene("Scene_Ringcounter-20");
		base = rcGameInterface->CreateScene("Scene_Wgauge_base");/*
		rcGameInterface->CreateScene("Scene_Wgauge_gauge2");*/
		lifeCount = rcGameInterface->CreateScene("Scene_Wgauge_count");/*
		rcGameInterface->CreateScene("Scene_Wgauge_gaugefull");
		rcGameInterface->CreateScene("Scene_Wgauge_gaugefull2");*/
		CSDCommon::PlayAnimation(*fullEffect, "gauge_full_effect", Chao::CSD::eMotionRepeatType_PlayOnce, 0, 1);
		CSDCommon::PlayAnimation(*fullEffect2, "gauge_full_effect", Chao::CSD::eMotionRepeatType_PlayOnce, 0, 1);
		fullMask->SetHideFlag(true);
		CSDCommon::PlayAnimation(*loseRing, "lost_ring", Chao::CSD::eMotionRepeatType_PlayOnce, 0, 0, 0);
		CSDCommon::PlayAnimation(*lifeCount, "10_figure", Chao::CSD::eMotionRepeatType_PlayOnce, 0, 1);
		CSDCommon::PlayAnimation(*ringCount, "no_ring", Chao::CSD::eMotionRepeatType_Loop, 0, 1);
		ringCount->SetPosition(HudSonicStage::xAspectOffset, HudSonicStage::yAspectOffset);
		CSDCommon::FreezeMotion(*loseRing);

		LetterboxHelper::ScaleScene(*soulGauge);
		LetterboxHelper::ScaleScene(*ringCount);
		LetterboxHelper::ScaleScene(*lifeCount);
		LetterboxHelper::ScaleScene(*base);
		LetterboxHelper::ScaleScene(*loseRing);
		LetterboxHelper::ScaleScene(*fullMask);
		float boostPos = 0;
		base->SetPosition(boostPos, 0);
		soulGauge->SetPosition(boostPos, 0);
		lifeCount->SetPosition(boostPos, 0);
		fullMask->SetPosition(boostPos, 0);
		ringCount->GetNode("100_figures")->SetPatternIndex(10);
		ringCount->GetNode("10_figures")->SetPatternIndex(10);
		ringCount->GetNode("single_figure")->SetPatternIndex(10);
	}
	else
	{
		soulGauge = rcGameInterface->CreateScene("Scene_Soul_gauge");
		ringCount = rcGameInterface->CreateScene("Scene_Ringcounter");
		fullEffect = rcGameInterface->CreateScene("Scene_Soul_gauge_add");
		loseRing = rcGameInterface->CreateScene("Scene_Ringcounter-20");
		float x = 640;
		float y = 363;
		loseRing->SetHideFlag(true);
		ringCount->SetPosition(x, y);
		ringCount->GetNode("denominator")->SetPosition(45, 0.013333334f);
		soulGauge->SetPosition(x, y);
		fullEffect->SetPosition(HudSonicStage::xAspectOffset, HudSonicStage::yAspectOffset);
		loseRing->SetPosition(x, y);

		if (isClassic) soulGauge->SetHideFlag(true);
		else soulGauge->SetHideFlag(false);
		CSDCommon::PlayAnimation(*soulGauge, "Loop", Chao::CSD::eMotionRepeatType_Loop, 1, 0);
		CSDCommon::PlayAnimation(*ringCount, "no_ring", Chao::CSD::eMotionRepeatType_Loop, 1, 0);
		CSDCommon::FreezeMotion(*fullEffect);
		CSDCommon::FreezeMotion(*loseRing);
		soulGauge->GetNode("Null_bluefire")->SetHideFlag(true);

	}

	flags &= ~(0x1 | 0x2 | 0x4 | 0x200 | 0x800); // Mask to prevent crash when game tries accessing the elements we disabled later on
	HudSonicStage::CreateScreen(This);
}

HOOK(void, __fastcall, CHudSonicStageUpdateParallel, 0x1098A50, Sonic::CGameObject* This, void* Edx, const hh::fnd::SUpdateInfo& in_rUpdateInfo)
{
	originalCHudSonicStageUpdateParallel(This, Edx, in_rUpdateInfo);
	HudSonicStage::ToggleScreen(*(bool*)0x1A430D8, This); // ms_IsRenderGameMainHud

	if (!spGameInterface)
		return;

	const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
	const size_t liveCountAddr = Common::GetMultiLevelAddress(0x1E66B34, { 0x4, 0x1B4, 0x7C, 0x9FDC });
	char text[256];

	if (Configuration::IsBeta) //E3
	{
		if (soulGauge)
		{
			if (!isClassic)
			{
				CSDCommon::PlayAnimation(*soulGauge, "gauge_up", Chao::CSD::eMotionRepeatType_PlayOnce, 1, playerContext->m_ChaosEnergy);
			}
			else
			{
				CSDCommon::PlayAnimation(*soulGauge, "gauge_up", Chao::CSD::eMotionRepeatType_PlayOnce, 1, playerContext->m_HorizontalVelocity.norm() / (playerContext->m_Is2DMode ? 45.0f : 90.0f) * 100.0f);
			}
		}

		if (fullMask)
			fullMask->SetHideFlag(!boostFull);

		if (fullEffect && playerContext)
		{
			float value = playerContext->m_ChaosEnergy;
			if (isClassic)
				value = playerContext->m_HorizontalVelocity.norm() / (playerContext->m_Is2DMode ? 45.0f : 90.0f) * 100.0f;

			if (value >= 100 && !boostFull)
			{
				CSDCommon::PlayAnimation(*fullEffect, "gauge_full", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
				CSDCommon::PlayAnimation(*fullEffect2, "gauge_full_effect", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
				boostFull = true;
			}
			else if (value < 100)
				boostFull = false;
		}

		if (ringCount && playerContext)
		{
			if (spinningRing)
			{
				if (repeatTime > 3)
				{
					if (ringIndex < 7)
						ringIndex++;
					else
						spinningRing = false;
				}
				if (ringIndex >= 7 && repeatTime <= 3)
				{
					ringIndex = 0;
					repeatTime++;
				}
				else if (repeatTime <= 3)
					ringIndex++;

			}

			ringCount->GetNode("Ring")->SetPatternIndex(ringIndex);

			sprintf(text, "%03d", playerContext->m_RingCount);
			if (prevRingCount != playerContext->m_RingCount && playerContext->m_RingCount < 1)
			{
				ringCount->GetNode("100_figures")->SetPatternIndex(10);
				ringCount->GetNode("10_figures")->SetPatternIndex(10);
				ringCount->GetNode("single_figure")->SetPatternIndex(10);
				CSDCommon::PlayAnimation(*ringCount, "no_ring", Chao::CSD::eMotionRepeatType_Loop, 1, 0);
			}
			else if (prevRingCount != playerContext->m_RingCount && playerContext->m_RingCount >= 1)
			{
				ringCount->GetNode("100_figures")->SetPatternIndex((int)text[0] - 48);
				ringCount->GetNode("10_figures")->SetPatternIndex((int)text[1] - 48);
				ringCount->GetNode("single_figure")->SetPatternIndex((int)text[2] - 48);
			}
			if (prevRingCount < playerContext->m_RingCount)
			{
				repeatTime = 0;
				spinningRing = true;
				CSDCommon::PlayAnimation(*ringCount, "no_ring", Chao::CSD::eMotionRepeatType_PlayOnce, 0, 0, 0);
				CSDCommon::PlayAnimation(*ringCount, "get_ring", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
			}
			else if (prevRingCount > playerContext->m_RingCount)
			{
				sprintf(text, "%02d", prevRingCount - playerContext->m_RingCount);
				loseRing->GetNode("-10_figures")->SetPatternIndex((int)text[0] - 48);
				loseRing->GetNode("-single_figure")->SetPatternIndex((int)text[1] - 48);
				CSDCommon::PlayAnimation(*loseRing, "lost_ring", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
				CSDCommon::PlayAnimation(*ringCount, "lost_ring_damage", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
			}
			prevRingCount = playerContext->m_RingCount;
		}
		if (lifeCount)
		{
			const size_t liveCountAddr = Common::GetMultiLevelAddress(0x1E66B34, { 0x4, 0x1B4, 0x7C, 0x9FDC });
			if (liveCountAddr)
			{
				sprintf(text, "%02d", *(size_t*)liveCountAddr);
				lifeCount->GetNode("10_figures")->SetPatternIndex((int)text[0] - 48);
				lifeCount->GetNode("single_figure")->SetPatternIndex((int)text[1] - 48);
			}
		}

	}
	else //Retail
	{
		if (soulGauge)
		{
			soulGauge->GetNode("Null_redfire")->SetScale(0.36, playerContext->m_ChaosEnergy / 100);
		}
		if (ringCount && playerContext)
		{
			if (loseRing->m_MotionDisableFlag)
				loseRing->SetHideFlag(true);
			if (spinningRing)
			{
				if (repeatTime > 3)
				{
					if (ringIndex < 7)
						ringIndex++;
					else
						spinningRing = false;
				}
				if (ringIndex >= 7 && repeatTime <= 3)
				{
					ringIndex = 0;
					repeatTime++;
				}
				else if (repeatTime <= 3)
					ringIndex++;

			}

			ringCount->GetNode("Ring")->SetPatternIndex(ringIndex);

			sprintf(text, "%03d", playerContext->m_RingCount);
			if (prevRingCount != playerContext->m_RingCount && playerContext->m_RingCount < 1)
			{
				ringCount->GetNode("100_figure")->SetPatternIndex(10);
				ringCount->GetNode("10_figure")->SetPatternIndex(10);
				ringCount->GetNode("single_figure")->SetPatternIndex(10);
				CSDCommon::PlayAnimation(*ringCount, "no_ring", Chao::CSD::eMotionRepeatType_Loop, 1, 0);
			}
			ringCount->GetNode("100_figure")->SetPatternIndex((int)text[0] - 48);
			ringCount->GetNode("10_figure")->SetPatternIndex((int)text[1] - 48);
			ringCount->GetNode("single_figure")->SetPatternIndex((int)text[2] - 48);
			if (prevRingCount < playerContext->m_RingCount)
			{
				repeatTime = 0;
				spinningRing = true;
				CSDCommon::PlayAnimation(*ringCount, "no_ring", Chao::CSD::eMotionRepeatType_PlayOnce, 0, 0, 0);
				CSDCommon::PlayAnimation(*ringCount, "get_ring", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
			}
			else if (prevRingCount > playerContext->m_RingCount)
			{
				loseRing->SetHideFlag(false);
				sprintf(text, "%02d", prevRingCount - playerContext->m_RingCount);
				loseRing->GetNode("ring_damage")->SetText(text);
				CSDCommon::PlayAnimation(*loseRing, "lost_ring_damage", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
			}
			prevRingCount = playerContext->m_RingCount;
		}

	}
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

void HudSonicStage::Install()
{
	INSTALL_HOOK(CHudSonicStageUpdate);
	INSTALL_HOOK(ProcMsgGetMissionCondition);
	INSTALL_HOOK(CHudSonicStageDelayProcessImp);
	INSTALL_HOOK(CHudSonicStageUpdateParallel);
	WRITE_MEMORY(0x16A467C, void*, CHudSonicStageRemoveCallback);
	WRITE_MEMORY(0x01703B30, float, 4);
	WRITE_MEMORY(0x01703B28, float, 3);
	WRITE_MEMORY(0x109B1A4, uint8_t, 0xE9, 0xDC, 0x02, 0x00, 0x00); // Disable lives
	WRITE_MEMORY(0x109B490, uint8_t, 0x90, 0xE9); // Disable time
	WRITE_MEMORY(0x109B5AD, uint8_t, 0x90, 0xE9); // Disable rings
	WRITE_MEMORY(0x109B8F5, uint8_t, 0x90, 0xE9); // Disable boost gauge
	WRITE_MEMORY(0x109BC88, uint8_t, 0x90, 0xE9); // Disable boost button
	INSTALL_HOOK(MsgStartCommonButtonSign);
}

/// <summary>
/// Originally from Unleashed HUD by Skyth and Brianuuu
/// </summary>
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