//CSD Object
boost::shared_ptr<Sonic::CGameObjectCSD> spGameplayColors, spMissionColors, spLifeColors, spGaugeEff;

//Projects
Chao::CSD::RCPtr<Chao::CSD::CProject> rcGameplayColors, rcMissionColors, rcLifeColors, rcGaugeEff;

//Scenes of projects (ideally keep them all separate from each project, unless they're used together)
Chao::CSD::RCPtr<Chao::CSD::CScene> rcPlayerCount, rcTimeCount, rcRingCount, rcBoostBar, rcBoostBarGlow,
rcWispContainer, rcScoreCount, rcBonusCount, rcSkillCount;
Chao::CSD::RCPtr<Chao::CSD::CScene> rcMissionTimer, rcMissionSecondP;
Chao::CSD::RCPtr<Chao::CSD::CScene> rcLife;

int glowAnimState;
float updateNumber;
//Parameters
size_t prevRingCount;
size_t itemCountDenominator;
bool isMission;
bool isBoosting, isUsingWisp, wispAcquired, isClassic, boostIntroPlayed, timeStarted, wispSet, readyGoNone, gettingWisp, removingWisp = false;
bool boostTransitionPlayed = true;
//Boost Shake
Hedgehog::Math::CVector2* shakeBoostValue;
Hedgehog::Math::CVector2* shakeBoostValuePixy;
Hedgehog::Math::CVector2* offsetAspect;
float boostShakePower;

//Would be used for wisp bar, but i dont know how to grab the time yet. Boowomp
float currentTimeRocket = 5;
float timeSinceStart = 0;
//Offset in case lives are disabled
float offset = 0;
int skillIndex = -1;
size_t flagsStuff;
bool lifeGoing = false;
int lifeAnim;

#pragma region XNCPStuff
void HudSonicStage::CreateScreen(Sonic::CGameObject* pParentGameObject)
{
	if (rcGameplayColors && !spGameplayColors)
		pParentGameObject->m_pMember->m_pGameDocument->AddGameObject(spGameplayColors = boost::make_shared<Sonic::CGameObjectCSD>(rcGameplayColors, 0.5f, "HUD", false), "main", pParentGameObject);
	if (rcMissionColors && !spMissionColors)
		pParentGameObject->m_pMember->m_pGameDocument->AddGameObject(spMissionColors = boost::make_shared<Sonic::CGameObjectCSD>(rcMissionColors, 0.5f, "HUD", false), "main", pParentGameObject);
	if (rcLifeColors && !spLifeColors)
		pParentGameObject->m_pMember->m_pGameDocument->AddGameObject(spLifeColors = boost::make_shared<Sonic::CGameObjectCSD>(rcLifeColors, 0.5f, "HUD", false), "main", pParentGameObject);
	if (rcGaugeEff && !spGaugeEff)
		pParentGameObject->m_pMember->m_pGameDocument->AddGameObject(spGaugeEff = boost::make_shared<Sonic::CGameObjectCSD>(rcGaugeEff, 0.5f, "HUD", false), "main", pParentGameObject);
}
void HudSonicStage::KillScreen()
{
	if (spGameplayColors)
	{
		spGameplayColors->SendMessage(spGameplayColors->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
		spGameplayColors = nullptr;
	}
	if (spMissionColors)
	{
		spMissionColors->SendMessage(spMissionColors->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
		spMissionColors = nullptr;
	}
	if (spGaugeEff)
	{
		spGaugeEff->SendMessage(spGaugeEff->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
		spGaugeEff = nullptr;
	}
	/*if (spLifeColors)
	{
		spLifeColors->SendMessage(spMissionColors->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
		spLifeColors = nullptr;
	}*/
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


void SetBoostGlowPosition(bool start) {

	Hedgehog::Math::CVector2 pos = rcBoostBar->GetNode("fulcrum_point_energy")->GetPosition();
	Hedgehog::Math::CVector2 posT = rcBoostBarGlow->GetNode("rt")->GetPosition();
	if (start) {

		rcBoostBarGlow->GetNode("fulcrum_point_energy")->SetPosition(pos.x() - 77, pos.y());
	}
	else
	{
		rcBoostBarGlow->GetNode("fulcrum_point_energy")->SetPosition(pos.x() - 81, pos.y());
	}
	rcBoostBarGlow->SetScale(1.47f, 1);
}
void ShakeBoostElements(uint32_t deltaTime, bool boostBrilliance = false)
{/*
	printf("deltatime %zu", deltaTime);*/
	rcWispContainer->GetNode("icon_brilliance")->SetHideFlag(!boostBrilliance);
	if (deltaTime % 2 == 0 && rcBoostBar && rcBoostBarGlow && rcWispContainer && boostIntroPlayed)
	{
		#pragma region Non-anim shake
		/*pos += rcBoostBar->GetNode("position")->GetPosition();*/
		/*rcBoostBar->SetPosition(CSDCommon::lerp(one->x(), shakeBoostValue->x(), 0.2f), CSDCommon::lerp(one->y(), shakeBoostValue->y(), 0.2f));*/
		/*rcWispContainer->SetPosition(CSDCommon::lerp(two->x(), shakeBoostValuePixy->x(), 0.2f), CSDCommon::lerp(two->y(), shakeBoostValuePixy->y(), 0.2f));*/
		/*float randomNum = (float)rand() / (float)RAND_MAX * PI;
		float randomNumWisp = (float)rand() / (float)RAND_MAX * PI;
		float shakingAmount = boostShakePower;
		Hedgehog::Math::CVector2* one = new Hedgehog::Math::CVector2(cos(randomNum) * shakingAmount, sin(randomNum) * shakingAmount);
		Hedgehog::Math::CVector2* two = new Hedgehog::Math::CVector2(cos(randomNumWisp) * shakingAmount, sin(randomNumWisp) * shakingAmount);
		if (shakeBoostValue == NULL && shakeBoostValuePixy == NULL)
		{
			shakeBoostValue = one; shakeBoostValuePixy = two;
		}		*/
#pragma endregion
		SetBoostGlowPosition(!wispAcquired);		

		if (rcWispContainer->m_MotionDisableFlag)
		{			
			CSDCommon::PlayAnimation(*rcWispContainer, "Usual_Anim", Chao::CSD::eMotionRepeatType_Loop, 1, 0);
		}
		if (rcBoostBar->m_MotionDisableFlag)
		{
			if (isUsingWisp)
				CSDCommon::PlayAnimation(*rcBoostBar, "Usual_Anim_2", Chao::CSD::eMotionRepeatType_Loop, 1, 0);
			else
				CSDCommon::PlayAnimation(*rcBoostBar, "Usual_Anim", Chao::CSD::eMotionRepeatType_Loop, 1, 0);
		}
		if (rcBoostBarGlow->m_MotionDisableFlag)
		{
			CSDCommon::PlayAnimation(*rcBoostBarGlow, "Usual_Anim", Chao::CSD::eMotionRepeatType_Loop, 1, 0);
		}
	}
}
void WispSet(int which)
{
	if (!rcWispContainer || !boostIntroPlayed)
		return;
	// 2 - Spike
	rcWispContainer->GetNode("icon_wisp")->SetPatternIndex(which);
}
void WispBarSet(bool wispInside)
{
	if (!boostIntroPlayed)
		return;
	if (wispInside)
	{
		if (rcWispContainer)		
			CSDCommon::PlayAnimation(*rcWispContainer, "get_wisp", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);			
		gettingWisp = true;
		removingWisp = false;
		
		if (rcBoostBarGlow)
		{
			SetBoostGlowPosition(false);
		}
	}
	else
	{
		if (rcWispContainer)		
			CSDCommon::PlayAnimation(*rcWispContainer, "mode_change_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		if(rcBoostBar)
			CSDCommon::PlayAnimation(*rcBoostBar, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 100);
		removingWisp = true;
		gettingWisp = false;		

		if (rcBoostBarGlow)
		{
			SetBoostGlowPosition(true);
		}
	}

}
void SetBoostValue(float value)
{
	if (isUsingWisp)
		return;
	if (!rcBoostBar || !rcBoostBarGlow)
		return;
	if (value == 0)
	{
		rcBoostBar->GetNode("gauge_energy")->SetHideFlag(true);
		rcBoostBarGlow->SetHideFlag(true);
	}
	else
	{
		rcBoostBar->GetNode("gauge_energy")->SetHideFlag(false);
		if (isBoosting)
			rcBoostBarGlow->SetHideFlag(false);
	}
	if (isBoosting)
	{
		float frameBefore = rcBoostBar->m_MotionFrame ;
		CSDCommon::PlayAnimation(*rcBoostBar, "gauge_energy_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, value);
		CSDCommon::PlayAnimation(*rcBoostBar, "Usual_Anim", Chao::CSD::eMotionRepeatType_Loop, 1, frameBefore);
		if (glowIntroOutro)
		{
			glowIntroOutro = rcBoostBarGlow->m_MotionDisableFlag;
		}
		else
		{			
			CSDCommon::PlayAnimation(*rcBoostBarGlow, "Size_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, value);
			CSDCommon::PlayAnimation(*rcBoostBarGlow, "Usual_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, frameBefore);
		}
		printf("\nFrame: %f", rcBoostBar->m_MotionFrame);
	}
	if (gettingWisp)
	{
		float frameBefore = rcBoostBar->m_MotionFrame;
		CSDCommon::PlayAnimation(*rcBoostBar, "gauge_energy", Chao::CSD::eMotionRepeatType_PlayOnce, 1, value);
		CSDCommon::PlayAnimation(*rcBoostBarGlow, "Size_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, value);
		CSDCommon::PlayAnimation(*rcBoostBar, "get_wisp", Chao::CSD::eMotionRepeatType_PlayOnce, 1, frameBefore);
		if (rcBoostBar->m_MotionDisableFlag)
			gettingWisp = false;
	}	
	if(!isBoosting && !gettingWisp)
	CSDCommon::PlayAnimation(*rcBoostBar, "gauge_energy", Chao::CSD::eMotionRepeatType_PlayOnce, 1, value);

	printf("\nBoosting: %d", isBoosting);
	printf("\nGettingWisp: %d", gettingWisp);

	//CSDCommon::PlayAnimation(*rcBoostBar, "size", Chao::CSD::eMotionRepeatType_PlayOnce, 1, value);
	//
}

void SetAllToIntroFirst()
{
	if (rcPlayerCount)
		HudSonicStage_IntroFirstFrame(rcPlayerCount);
	if (rcLife)
		HudSonicStage_IntroFirstFrame(rcLife);
	if (rcTimeCount)
		HudSonicStage_IntroFirstFrame(rcTimeCount);
	if (rcRingCount)
		HudSonicStage_IntroFirstFrame(rcRingCount);
	if (rcBoostBar)
	{
		SetBoostValue(Sonic::Player::CPlayerSpeedContext::GetInstance()->m_ChaosEnergy);
		HudSonicStage_IntroFirstFrame(rcBoostBar);
	}
	if (rcWispContainer)
		HudSonicStage_IntroFirstFrame(rcWispContainer);
	if (rcScoreCount)
		HudSonicStage_IntroFirstFrame(rcScoreCount);
}

//REMEMBER TO CALL NULLPTR HERE!!!!
void __fastcall CHudSonicStageRemoveCallback(Sonic::CGameObject* This, void*, Sonic::CGameDocument* pGameDocument)
{
	HudSonicStage::KillScreen();

	Chao::CSD::CProject::DestroyScene(rcGameplayColors.Get(), rcPlayerCount);
	Chao::CSD::CProject::DestroyScene(rcGameplayColors.Get(), rcTimeCount);
	Chao::CSD::CProject::DestroyScene(rcGameplayColors.Get(), rcRingCount);
	Chao::CSD::CProject::DestroyScene(rcGameplayColors.Get(), rcBoostBar);
	Chao::CSD::CProject::DestroyScene(rcGameplayColors.Get(), rcWispContainer);
	Chao::CSD::CProject::DestroyScene(rcGameplayColors.Get(), rcScoreCount);
	Chao::CSD::CProject::DestroyScene(rcMissionColors.Get(), rcMissionTimer);
	Chao::CSD::CProject::DestroyScene(rcLifeColors.Get(), rcLife);
	Chao::CSD::CProject::DestroyScene(rcGaugeEff.Get(), rcBoostBarGlow);

	isBoosting, isUsingWisp, wispAcquired, isClassic, boostIntroPlayed, timeStarted, wispSet, readyGoNone, gettingWisp, removingWisp = false;
	boostTransitionPlayed = true;
	rcGameplayColors = nullptr;
	rcMissionColors = nullptr;
	rcLifeColors = nullptr;
	rcGaugeEff = nullptr;
	isMission = false;
	timeSinceStart = 0;
	timeStarted = false;
	boostIntroPlayed = false;
	isUsingWisp = false;
	isBoosting = false;
	wispAcquired = false;
	isClassic = false;
	readyGoNone = true;
	skillIndex = -1;
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

	auto spCsdProject = wrapper.GetCsdProject("ui_gameplay_colors");
	rcGameplayColors = spCsdProject->m_rcProject;
	spCsdProject = wrapper.GetCsdProject("ui_gameplay_multi");
	rcMissionColors = spCsdProject->m_rcProject;
	spCsdProject = wrapper.GetCsdProject("ui_1up");
	rcLifeColors = spCsdProject->m_rcProject;
	spCsdProject = wrapper.GetCsdProject("ui_burndown-timer");
	rcGaugeEff = spCsdProject->m_rcProject;
	Configuration::Read();
	boostShakePower = Configuration::GaugeShakeAmountXNCP;
	isMission = !Common::IsCurrentStageBossBattle() && (Common::GetCurrentStageID() & (SMT_Mission1 | SMT_Mission2 | SMT_Mission3 | SMT_Mission4 | SMT_Mission5));

	size_t& flags = ((size_t*)This)[151];
	flagsStuff = flags;
	//If shadow fight
	if (Common::GetCurrentStageID() == SMT_bsd || isMission)
	{
		rcMissionTimer = rcMissionColors->CreateScene("info_2");
		CSDCommon::FreezeMotion(*rcMissionTimer);
		CSDCommon::IntroAnim(rcMissionTimer);
		if (Common::GetCurrentStageID() == SMT_bsd)
		{

			//Fixes the UI because the SCU multi xncp is for some reason kinda broken with scaling
			rcMissionSecondP = rcMissionColors->CreateScene("info_1_2p");
			rcMissionSecondP->SetPosition(580, 45);
			rcMissionSecondP->GetNode("bg_r_f")->SetPosition(10, 0);
			rcMissionSecondP->SetScale(0.53f, 0.53f);
			rcMissionSecondP->GetNode("bg_l")->SetScale(1, 1.2f);
			rcMissionSecondP->GetNode("bg_r_f")->SetScale(1, 1.2f);
			rcMissionSecondP->GetNode("icon_sonic_2p_0")->SetHideFlag(true);
			rcMissionSecondP->GetNode("icon_sonic_2p_3")->SetHideFlag(true);
			rcMissionSecondP->GetNode("icon_sonic_shade")->SetHideFlag(true);
			rcMissionSecondP->GetNode("num_life")->SetPosition(-140, -118);
			rcMissionSecondP->GetNode("icon_sonic_2p_1")->SetScale(1.15f, 1.15f);
			rcMissionSecondP->GetNode("color_2p")->SetPosition(0, 50000);

			rcMissionTimer->GetNode("num_time")->SetPosition(0, -120);

			CSDCommon::FreezeMotion(*rcMissionSecondP);
			CSDCommon::IntroAnim(rcMissionSecondP);
		}
	}
	if (flags & 0x1 && *(uint8_t*)0x1098C82 != 0xEB || Common::GetCurrentStageID() == SMT_bsd) // Lives
	{
		rcPlayerCount = rcGameplayColors->CreateScene("info_1");
		rcLife = rcLifeColors->CreateScene("1up");
		offset = 0;
	}
	else
		offset = -48;
	if (flags & 0x2) // Time
	{
		rcTimeCount = rcGameplayColors->CreateScene("info_2");
		rcTimeCount->SetPosition(0, offset);
	}
	if (flags & 0x4 || Common::GetCurrentStageID() == SMT_bsd) // Rings
	{
		rcRingCount = rcGameplayColors->CreateScene("ring");
		rcRingCount->SetPosition(0, offset);
	}
	isClassic = !flags & 0x200;
	if (flags) // Boost Gauge
	{
		rcBoostBar = rcGameplayColors->CreateScene("gauge_energy");
		CSDCommon::PlayAnimation(*rcBoostBar, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 100);
		//Create, set position, and fix boost bar glow to work with the 16:9 gameplay XNCP
		rcBoostBarGlow = rcGaugeEff->CreateScene("gauge_energy_2_eff");

		Hedgehog::Math::CVector2 posT = rcBoostBarGlow->GetNode("rt")->GetPosition();
		rcBoostBarGlow->GetNode("rt")->SetPosition(posT.x() + 5, posT.y());
		SetBoostGlowPosition(true);

		rcWispContainer = rcGameplayColors->CreateScene("pixy", "Intro_Anim");
		rcWispContainer->m_MotionRepeatType = Chao::CSD::eMotionRepeatType_PlayOnce;
		if (isClassic)
		{
			rcBoostBar->SetPosition(1000000, 0);
		}
	}
	flags &= ~(0x1 | 0x2 | 0x4 | 0x200 | 0x800); // Mask to prevent crash when game tries accessing the elements we disabled later on


	if (ScoreGenerationsAPI::IsAttached() && !ScoreGenerationsAPI::IsStageForbidden()) // Score
	{
		rcScoreCount = rcGameplayColors->CreateScene("info_score");
	}
	SetAllToIntroFirst();
	HudSonicStage::CreateScreen(This);
}

HOOK(void, __fastcall, CHudSonicStageUpdateParallel, 0x1098A50, Sonic::CGameObject* This, void* Edx, const hh::fnd::SUpdateInfo& in_rUpdateInfo)
{
	originalCHudSonicStageUpdateParallel(This, Edx, in_rUpdateInfo);
	HudSonicStage::ToggleScreen(*(bool*)0x1A430D8, This); // ms_IsRenderGameMainHud

	if (!spGameplayColors && !spMissionColors)
		return;


	char text[256];
	size_t rowIndex = 1;
	const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
	updateNumber = in_rUpdateInfo.DeltaTime;
	if (rcLife && lifeGoing)
	{
		switch (lifeAnim)
		{
		case 1:
		{
			//Life intro_anim
			if (rcLife->m_MotionDisableFlag == 1)
			{
				lifeAnim = 2;
				CSDCommon::PlayAnimation(*rcLife, "countup_Anim_1", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, false, false);
			}

			break;
		}
		case 2: {
			//Life 1up
			if (rcLife->m_MotionDisableFlag == 1)
			{
				lifeAnim = 3;
				CSDCommon::PlayAnimation(*rcLife, "Outro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, false, false);
			}
		}
		case 3:
		{
			//Life outro_anim
			if (rcLife->m_MotionDisableFlag == 1)
			{
				lifeGoing = false;
			}
		}
		}
	}
	if (timeSinceStart < 3.5f)
	{
		timeSinceStart += in_rUpdateInfo.DeltaTime;
		if (timeSinceStart >= 0.5f && readyGoNone)
			timeSinceStart = 3.5f;
	}
	else if (timeSinceStart >= 3.5f && !timeStarted)
	{
		//i honestly dont know why this works
		SetBoostValue(playerContext->m_ChaosEnergy);

		if (rcPlayerCount)
			CSDCommon::IntroAnim(rcPlayerCount);
		if (rcTimeCount)
			CSDCommon::IntroAnim(rcTimeCount);
		if (rcRingCount)
			CSDCommon::IntroAnim(rcRingCount);
		if (rcBoostBar)
			CSDCommon::IntroAnim(rcBoostBar);
		if (rcScoreCount)
			CSDCommon::IntroAnim(rcScoreCount);
		if (rcWispContainer)
			CSDCommon::IntroAnim(rcWispContainer);

		timeStarted = true;
	}
	if (timeStarted && !boostIntroPlayed && rcBoostBar)
	{
		if (rcBoostBar)
			boostIntroPlayed = rcBoostBar->m_MotionDisableFlag == 1;
		else
			boostIntroPlayed = true;
	}
	if (boostIntroPlayed && !wispSet)
	{
		if (skillIndex != -1)
		{
			WispSet(skillIndex);
			WispBarSet(true);
			wispSet = true;
		}
		else
			wispSet = true;
	}
	if (rcPlayerCount)
	{
		const size_t liveCountAddr = Common::GetMultiLevelAddress(0x1E66B34, { 0x4, 0x1B4, 0x7C, 0x9FDC });
		if (liveCountAddr)
		{
			sprintf(text, "%02d", *(size_t*)liveCountAddr);
			rcPlayerCount->GetNode("num_life")->SetText(text);
		}
	}

	if (rcTimeCount)
	{
		size_t minutes, seconds, milliseconds;
		GetTime(**This->m_pMember->m_pGameDocument, &minutes, &seconds, &milliseconds);

		sprintf(text, "%02d:%02d.%02d", minutes, seconds, milliseconds);
		rcTimeCount->GetNode("num_time")->SetText(text);
		//Should add the timeout feature from colors here

	}
	if (rcMissionTimer)
	{
		size_t minutes, seconds, milliseconds;
		GetTime(**This->m_pMember->m_pGameDocument, &minutes, &seconds, &milliseconds);

		sprintf(text, "%02d:%02d.%02d", minutes, seconds, milliseconds);
		rcMissionTimer->GetNode("num_time")->SetText(text);
	}
	if (rcScoreCount)
	{
		sprintf(text, "%08d", ScoreGenerationsAPI::GetScore());
		rcScoreCount->GetNode("num_score")->SetText(text);
	}

	/*printf("\n");
	printf(->GetStateName().c_str());*/
	if (rcBoostBarGlow)
		rcBoostBarGlow->SetHideFlag(!isBoosting);


	if (rcBoostBar && rcWispContainer)
	{
		//Boosting Anims
		if (!isUsingWisp)
		{
			///Boosting Anims

			if (playerContext->StateFlag(eStateFlag_Boost))
			{
				if (!isBoosting)
				{
					CSDCommon::PlayAnimation(*rcBoostBarGlow, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
				}
				ShakeBoostElements(in_rUpdateInfo.Frame);
				isBoosting = true;
			}
			else
			{/*
				rcBoostBar->SetPosition(0, 0);
				rcWispContainer->SetPosition(0, 0);
				rcBoostBarGlow->SetPosition(0, 0);*/
				if (isBoosting)
				{
					if (wispAcquired)
						CSDCommon::PlayAnimation(*rcWispContainer, "mode_change", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 100);
					else
						CSDCommon::PlayAnimation(*rcWispContainer, "mode_change_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 100);
				}

				if (isBoosting)
				{
					CSDCommon::PlayAnimation(*rcBoostBarGlow, "Outro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
				}
				isBoosting = false;
			}
			if (boostIntroPlayed && !isUsingWisp)
				SetBoostValue(playerContext->m_ChaosEnergy);

		}
		//Wisp Conversion
		Hedgehog::Base::CSharedString stateCheck = playerContext->m_pPlayer->m_StateMachine.GetCurrentState()->GetStateName();
		std::string stateCheckS(stateCheck.c_str());

		if ((stateCheckS == "RocketLaunch" || stateCheckS == "RocketIdle") || ((stateCheckS.find("Spike") != std::string::npos) && stateCheckS != "TransformSpike"))
		{
			printf("\n");
			printf("%d", currentTimeRocket);
			printf("RunningAnimationWisp");
			if (isClassic)
				rcBoostBar->SetHideFlag(false);
			if (!isUsingWisp)
			{
				CSDCommon::FreezeMotion(*rcBoostBar);
				CSDCommon::PlayAnimation(*rcBoostBar, "gauge_time", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 500.0f);
				CSDCommon::PlayAnimation(*rcBoostBar, "mode_change", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
			}
			isUsingWisp = true;

			ShakeBoostElements(in_rUpdateInfo.Frame, true);
		}
		else
		{
			if (isUsingWisp && boostTransitionPlayed)
			{
				wispAcquired = false;
				if (isClassic)
					rcBoostBar->SetHideFlag(true);

				boostTransitionPlayed = false;
				WispBarSet(false);
				CSDCommon::PlayAnimation(*rcBoostBar, "mode_change_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
			}
			if (!boostTransitionPlayed)
			{
				boostTransitionPlayed = rcBoostBar->m_MotionDisableFlag;
				isUsingWisp = !boostTransitionPlayed;
			}
		}
	}
	if (rcRingCount && playerContext)
	{
		sprintf(text, "%04d", playerContext->m_RingCount);
		rcRingCount->GetNode("num_ring")->SetText(text);

		if (prevRingCount < playerContext->m_RingCount && boostIntroPlayed)
		{
			CSDCommon::PlayAnimation(*rcRingCount, "get_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		}

		prevRingCount = playerContext->m_RingCount;
	}
}
HOOK(void, __fastcall, MsgChangeCustomHud, 0x10947A0, Sonic::CGameObject* This, void* Edx, hh::fnd::Message& in_rMsg)
{
	const size_t iconType = *(size_t*)((char*)&in_rMsg + 16);
	const size_t iconTypeT = *(size_t*)((char*)&in_rMsg + 8);
	const size_t spriteCount = *(size_t*)((char*)&in_rMsg + 20);
	WispSet(iconType);
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
	skillIndex = iconType;

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

	if (rcWispContainer && !isUsingWisp)
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
	}
}
HOOK(void, __stdcall, CPlayerGetLife, 0xE75520, Sonic::Player::CPlayerContext* context, int lifeCount, bool playSound)
{
	/*originalCPlayerGetLife(context, lifeCount, playSound);*/

	if (lifeCount > 0)
	{
		if (rcLife)
		{
			rcLife->GetNode("icon")->SetPosition(1, 1);
			CSDCommon::PlayAnimation(*rcLife, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, false, false);
		}
		lifeGoing = true;
		lifeAnim = 1;
		if (playSound)
		{
			context->PlaySound(4001009, 0);
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
//Brianuu/Skyth
void HudSonicStage_CalculateAspectOffsets()
{
	if (*(size_t*)0x6F23C6 != 0x75D8C0D9) // Widescreen Support
	{
		const float aspect = (float)*(size_t*)0x1DFDDDC / (float)*(size_t*)0x1DFDDE0;

		if (aspect * 9.0f > 16.0f)
		{
			offsetAspect = new Hedgehog::Math::CVector2(720.0f * aspect - 1280.0f, 0.0f);
		}
		else
		{
			offsetAspect = new Hedgehog::Math::CVector2(0, 1280.0f / aspect - 720.0f);
		}
	}
	else
	{
		offsetAspect = new Hedgehog::Math::CVector2(0, 0);
	}
}
/*
	t;*/
HOOK(int, __fastcall, MsgRestartStage, 0xE76810, Sonic::CGameObject* This, void* edx, int a2)
{
	timeSinceStart = 0;
	boostIntroPlayed = false;
	timeStarted = false;

	SetAllToIntroFirst();
	return originalMsgRestartStage(This, edx, a2);
}
HOOK(int, __fastcall, MsgStartMode, 0x109DAA0, Sonic::CGameObject* This)
{
	readyGoNone = false;
#if DEBUG
	printf("\n\nStarting Level\n");
#endif
	return originalMsgStartMode(This);
}


void HudSonicStage::Install()
{
	INSTALL_HOOK(CHudSonicStageUpdate);
	INSTALL_HOOK(MsgStartMode);
	INSTALL_HOOK(MsgRestartStage);
	/*INSTALL_HOOK(TestRestart);*/

	INSTALL_HOOK(ProcMsgGetMissionCondition);
	INSTALL_HOOK(CHudSonicStageDelayProcessImp);
	INSTALL_HOOK(CHudSonicStageUpdateParallel);
	INSTALL_HOOK(MsgChangeCustomHud);
	INSTALL_HOOK(ProcMsgChangeWispHud);
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