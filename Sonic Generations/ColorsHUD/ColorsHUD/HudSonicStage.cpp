///TODO:
/// Fix up red rings method
/// Fix externalcontrol with wisps
/// Fix super icon and skills contradicting each other
/// 

struct ScoreEntry {
	int type;
	int sceneIndex;
	unsigned long frame;
	bool playedSwitch;
	bool timeout;
};
//CSD Object
boost::shared_ptr<Sonic::CGameObjectCSD> spGameplayColors, spMissionColors, spLifeColors, spGaugeEff;

//Projects
Chao::CSD::RCPtr<Chao::CSD::CProject> rcGameplayColors, rcMissionColors, rcLifeColors, rcGaugeEff;

//Scenes of projects (ideally keep them all separate from each project, unless they're used together)
Chao::CSD::RCPtr<Chao::CSD::CScene> rcPlayerCount, rcTimeCount, rcRingCount, rcBoostBar, rcBoostBarGlow, rcWispPlume, rcWispSpark, rcWispEff1,
rcWispContainer, rcScoreCount, rcBonusCount, rcSkillCount, rcStarRing, rcSuperEffect;
Chao::CSD::RCPtr<Chao::CSD::CScene> rcMissionTimer, rcMissionSecondP;
Chao::CSD::RCPtr<Chao::CSD::CScene> rcLife, rcSpecialRing;
std::vector<Chao::CSD::RCPtr<Chao::CSD::CScene>> rcScoreBonusEntries;
std::vector<ScoreEntry> scoreEntries;


//Parameters
int wispType;
bool isMission;
bool scoreEnabled;
int glowAnimState;
size_t prevRingCount;
bool playingWispEffect;
float timerSpike, timerRocket;
int HudSonicStage::redRingCount;
bool boostTransitionPlayed = true;
int HudSonicStage::prevRedRingCount;
Hedgehog::Math::CVector2* offsetAspect;
bool isBoosting, currentlyUsingWisp, hasWisp, isClassic, boostIntroPlayed, timeStarted = false;
bool wispBarUpdated, readyGoNone, gettingWisp, removingWisp, passedCheckpoint = false;

float timeSinceStart = 0;

//Offset in case lives are disabled
float livesOffset = 0;
int currentSkillIndex = -1;
bool gotNewLife = false;
int currentLifeAnim;

Statistics::Totals prevScoreTotals;
//Red ring acquired anim
DWORD* redRingsMysterySource;
int redStarCount = 0;
bool spinningRedRing = false;
int animStateSRing = 0;
int spinTimes = 0;

//Score Bonus
int maxScoreEntries = 5;
int scoreEntrySpacing;
int currentlyActiveEntries;

inline FUNCTION_PTR(int, __fastcall, GetSpecialRingCount, 0xD591B0, int a1, int unused, DWORD* a3);


#pragma region XNCPStuff
void HudSonicStage::CreateScreen(Sonic::CGameObject* pParentGameObject)
{
	Configuration::Read();
	if (rcGameplayColors && !spGameplayColors)
		pParentGameObject->m_pMember->m_pGameDocument->AddGameObject(spGameplayColors = boost::make_shared<Sonic::CGameObjectCSD>(rcGameplayColors, 0.5f, "HUD", false), "main", pParentGameObject);
	if (rcMissionColors && !spMissionColors)
		pParentGameObject->m_pMember->m_pGameDocument->AddGameObject(spMissionColors = boost::make_shared<Sonic::CGameObjectCSD>(rcMissionColors, 0.5f, "HUD", false), "main", pParentGameObject);
	if (rcLifeColors && !spLifeColors)
		pParentGameObject->m_pMember->m_pGameDocument->AddGameObject(spLifeColors = boost::make_shared<Sonic::CGameObjectCSD>(rcLifeColors, 0.5f, "HUD", false), "main", pParentGameObject);
	if (rcGaugeEff && !spGaugeEff && Configuration::IsColorsOG)
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
	if (spLifeColors)
	{
		spLifeColors->SendMessage(spLifeColors->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
		spLifeColors = nullptr;
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
void SetBoostGlowPosition(bool start) {

	Hedgehog::Math::CVector2 pos = rcBoostBar->GetNode("fulcrum_point_energy")->GetPosition();
	Hedgehog::Math::CVector2 posT = rcBoostBarGlow->GetNode("rt")->GetPosition();
	Configuration::Read();
	if (Configuration::IsColorsOG)
	{
		if (start) {

			rcBoostBarGlow->GetNode("fulcrum_point_energy")->SetPosition(pos.x() - 77, pos.y());
		}
		else
		{
			rcBoostBarGlow->GetNode("fulcrum_point_energy")->SetPosition(pos.x() - 81, pos.y());
		}
		rcBoostBarGlow->SetScale(1.47f, 1);
	}
	else
	{
		if (start)
		{
			rcBoostBarGlow->GetNode("fulcrum_point_energy")->SetPosition(pos.x() - 57.8f, pos.y() + 21.13f);
		}
		else
		{
			rcBoostBarGlow->GetNode("fulcrum_point_energy")->SetPosition(pos.x() - 64.1f, pos.y() + 21.13f);
		}
		rcBoostBarGlow->SetScale(2.26f, 1);
	}
}
void SetWispEffectPosition(bool rocket)
{
	if (!playingWispEffect)
	{
		CSDCommon::FreezeMotion(*rcWispPlume);
		CSDCommon::FreezeMotion(*rcWispSpark);
		CSDCommon::FreezeMotion(*rcWispEff1);
		CSDCommon::PlayAnimation(*rcWispPlume, rocket ? "Usual_Orange" : "Usual_Purple", Chao::CSD::eMotionRepeatType_Loop, 1, 0);
		CSDCommon::PlayAnimation(*rcWispSpark, rocket ? "Usual_Orange" : "Usual_Purple", Chao::CSD::eMotionRepeatType_Loop, 1, 0);
		CSDCommon::PlayAnimation(*rcWispEff1, rocket ? "Usual_Orange" : "Usual_Purple", Chao::CSD::eMotionRepeatType_Loop, 1, 0);
		playingWispEffect = true;
	}
	auto pos = rcBoostBar->GetNode("time_end")->GetPosition();
	rcWispPlume->SetPosition(pos.x() + 40, pos.y());
	rcWispSpark->SetPosition(pos.x() + 40, pos.y());
	rcWispEff1->SetPosition(pos.x(), pos.y());

}
void ShakeBoostElements(float deltaTime, bool boostBrilliance = false)
{
	rcWispContainer->GetNode("icon_brilliance")->SetHideFlag(!boostBrilliance);
	if (rcBoostBar && rcBoostBarGlow && rcWispContainer && boostIntroPlayed)
	{
		SetBoostGlowPosition(!hasWisp);

		if (rcWispContainer->m_MotionDisableFlag)
		{
			float frameBefore = rcWispContainer->m_MotionFrame;
			CSDCommon::PlayAnimation(*rcWispContainer, "Usual_Anim", Chao::CSD::eMotionRepeatType_Loop, 1, frameBefore);
		}
		auto inputState = Sonic::CInputState::GetInstance();
		auto inputPtr = &inputState->m_PadStates[inputState->m_CurrentPadStateIndex];

		if (currentlyUsingWisp)
		{
			//Wisp Usage Bar
			float frameBefore = rcBoostBar->m_MotionFrame;
			CSDCommon::PlayAnimation(*rcBoostBar, "Usual_Anim_2", Chao::CSD::eMotionRepeatType_Loop, 1, frameBefore);
			CSDCommon::PlayAnimation(*rcBoostBar, "mode_change", Chao::CSD::eMotionRepeatType_PlayOnce, 1, frameBefore);

			if (wispType == 0) //Spike 
				CSDCommon::PlayAnimation(*rcBoostBar, "gauge_time", Chao::CSD::eMotionRepeatType_Loop, 1, (timerSpike -= deltaTime) / 15 * 100);
			else if (wispType == 1 && !inputPtr->IsDown(Sonic::eKeyState_Y)) //Rocket
				CSDCommon::PlayAnimation(*rcBoostBar, "gauge_time", Chao::CSD::eMotionRepeatType_Loop, 1, (timerRocket -= deltaTime) / 3 * 100);

			CSDCommon::PlayAnimation(*rcBoostBar, "Usual_Anim_2", Chao::CSD::eMotionRepeatType_Loop, 1, frameBefore);
			SetWispEffectPosition(wispType == 1);
		}
		else if (!currentlyUsingWisp && !deltaTime % 2) //The reason why we want to check for the delta time is because otherwise it would be playing 3 anims at once
		{
			CSDCommon::PlayAnimation(*rcBoostBar, "Usual_Anim", Chao::CSD::eMotionRepeatType_Loop, 1, 0);
			playingWispEffect = false;
		}

		CSDCommon::PlayAnimation(*rcBoostBarGlow, "Usual_Anim", Chao::CSD::eMotionRepeatType_Loop, 1, 0);
		/*if (rcBoostBarGlow->m_MotionDisableFlag && glowAnimState == 2)
		{
			CSDCommon::PlayAnimation(*rcBoostBarGlow, "Usual_Anim", Chao::CSD::eMotionRepeatType_Loop, 1, 0);
		}*/
	}
}
void SetWispContainer(int which)
{
	if (!rcWispContainer || !boostIntroPlayed)
		return;
	// 2 - Spike
	if (gettingWisp)
		rcWispContainer->GetNode("icon_wisp")->SetPatternIndex(which);
	else
	{
		CSDCommon::PlayAnimation(*rcWispContainer, "mode_change", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		rcWispContainer->GetNode("icon_wisp")->SetPatternIndex(which);
	}
}
void WispBarSet(bool wispInside, bool end = false)
{
	if (!boostIntroPlayed)
		return;
	float frame = end ? 100 : 0;
	if (wispInside)
	{
		if (rcWispContainer)
			CSDCommon::PlayAnimation(*rcWispContainer, "get_wisp", Chao::CSD::eMotionRepeatType_PlayOnce, 1, frame);
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
			CSDCommon::PlayAnimation(*rcWispContainer, "mode_change_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, frame);
		if (rcBoostBar)
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
	if (currentlyUsingWisp)
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
		float frameBefore = rcBoostBar->m_MotionFrame;
		DebugDrawText::log((std::string("Frame Boost Before") + std::to_string(frameBefore)).c_str(), 0);
		CSDCommon::PlayAnimation(*rcBoostBar, "gauge_energy_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, value);
		CSDCommon::PlayAnimation(*rcBoostBar, "Usual_Anim", Chao::CSD::eMotionRepeatType_Loop, 1, frameBefore);


		CSDCommon::PlayAnimation(*rcBoostBarGlow, "Size_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, value);
		CSDCommon::PlayAnimation(*rcBoostBarGlow, "Usual_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, frameBefore);


		printf("\nFrame: %f", rcBoostBar->m_MotionFrame);
	}
	if (gettingWisp)
	{
		float frameBefore = rcBoostBar->m_MotionFrame;
		CSDCommon::PlayAnimation(*rcBoostBar, "gauge_energy", Chao::CSD::eMotionRepeatType_PlayOnce, 1, value);
		CSDCommon::PlayAnimation(*rcBoostBarGlow, "Size_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, value);
		CSDCommon::PlayAnimation(*rcBoostBar, "get_wisp", Chao::CSD::eMotionRepeatType_PlayOnce, 1, frameBefore);
		if (rcBoostBar->m_MotionDisableFlag && rcWispContainer->m_MotionDisableFlag)
		{
			gettingWisp = false; CSDCommon::PlayAnimation(*rcWispContainer, "get_Usual_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, value);
		}
	}

	if (!isBoosting && !gettingWisp)
		CSDCommon::PlayAnimation(*rcBoostBar, "gauge_energy", Chao::CSD::eMotionRepeatType_PlayOnce, 1, value);


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
	if (rcStarRing)
		HudSonicStage_IntroFirstFrame(rcStarRing);
}

void SetRedStarCount(int count)
{
	if (rcStarRing)
	{
		redStarCount = count;
		rcStarRing->GetNode("ring_0")->SetPatternIndex((int)count >= 1);
		rcStarRing->GetNode("ring_1")->SetPatternIndex((int)count >= 2);
		rcStarRing->GetNode("ring_2")->SetPatternIndex((int)count >= 3);
		rcStarRing->GetNode("ring_3")->SetPatternIndex((int)count >= 4);
		rcStarRing->GetNode("ring_4")->SetPatternIndex((int)count >= 5);
	}
}

//REMEMBER TO CALL NULLPTR HERE!!!!
void __fastcall CHudSonicStageRemoveCallback(Sonic::CGameObject* This, void*, Sonic::CGameDocument* pGameDocument)
{
	WispBarSet(false, true);
	HudSonicStage::KillScreen();
	Chao::CSD::CProject::DestroyScene(rcGameplayColors.Get(), rcPlayerCount);
	Chao::CSD::CProject::DestroyScene(rcGameplayColors.Get(), rcTimeCount);
	Chao::CSD::CProject::DestroyScene(rcGameplayColors.Get(), rcRingCount);
	Chao::CSD::CProject::DestroyScene(rcGameplayColors.Get(), rcBoostBar);
	Chao::CSD::CProject::DestroyScene(rcGameplayColors.Get(), rcWispContainer);
	Chao::CSD::CProject::DestroyScene(rcGameplayColors.Get(), rcScoreCount);
	Chao::CSD::CProject::DestroyScene(rcGameplayColors.Get(), rcStarRing);
	Chao::CSD::CProject::DestroyScene(rcMissionColors.Get(), rcWispEff1);
	Chao::CSD::CProject::DestroyScene(rcMissionColors.Get(), rcWispPlume);
	Chao::CSD::CProject::DestroyScene(rcMissionColors.Get(), rcWispSpark);
	Chao::CSD::CProject::DestroyScene(rcLifeColors.Get(), rcLife);
	Chao::CSD::CProject::DestroyScene(rcLifeColors.Get(), rcSpecialRing);
	Chao::CSD::CProject::DestroyScene(rcGaugeEff.Get(), rcBoostBarGlow);
	for (size_t i = 0; i < rcScoreBonusEntries.size(); i++)
	{
		Chao::CSD::CProject::DestroyScene(rcGameplayColors.Get(), rcScoreBonusEntries[i]);
	}
	scoreEntries.clear();

	isBoosting, currentlyUsingWisp, hasWisp, isClassic, boostIntroPlayed, timeStarted, wispBarUpdated, readyGoNone, gettingWisp, removingWisp, isMission, passedCheckpoint = false;
	boostTransitionPlayed, readyGoNone = true;
	timeSinceStart = 0;
	currentSkillIndex = -1;

	//Hide Projects
	rcGameplayColors = nullptr;
	rcMissionColors = nullptr;
	rcLifeColors = nullptr;
	rcGaugeEff = nullptr;
}

HOOK(void, __fastcall, CHudSonicStageDelayProcessImp, 0x109A8D0, Sonic::CGameObject* This)
{
	ScoreGenerationsAPI::SetVisibility(false);
	originalCHudSonicStageDelayProcessImp(This);
	CHudSonicStageRemoveCallback(This, nullptr, nullptr);
	Configuration::Read();

	Sonic::CCsdDatabaseWrapper wrapper(This->m_pMember->m_pGameDocument->m_pMember->m_spDatabase.get());

	auto spCsdProject = wrapper.GetCsdProject("ui_gameplay_colors");
	rcGameplayColors = spCsdProject->m_rcProject;
	spCsdProject = wrapper.GetCsdProject("ui_gameplay_multi");
	rcMissionColors = spCsdProject->m_rcProject;
	spCsdProject = wrapper.GetCsdProject("ui_1up");
	rcLifeColors = spCsdProject->m_rcProject;
	if (Configuration::IsColorsOG) {

		spCsdProject = wrapper.GetCsdProject("ui_burndown-timer");
		rcGaugeEff = spCsdProject->m_rcProject;
	}
	Configuration::Read();
	isMission = !Common::IsCurrentStageBossBattle() && (Common::GetCurrentStageID() & (SMT_Mission1 | SMT_Mission2 | SMT_Mission3 | SMT_Mission4 | SMT_Mission5));

	size_t& flags = ((size_t*)This)[151];
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
		livesOffset = 0;
	}
	else
		livesOffset = -48;

	if (flags & 0x2) // Time
	{
		rcTimeCount = rcGameplayColors->CreateScene("info_2");
		rcTimeCount->SetPosition(0, livesOffset);
	}
	if (flags & 0x4 || Common::GetCurrentStageID() == SMT_bsd || Common::GetCurrentStageID() == SMT_blb) // Rings
	{
		rcRingCount = rcGameplayColors->CreateScene("ring");
		rcRingCount->SetPosition(0, livesOffset);
	}
	isClassic = !(flags & 0x200);
	if (flags) // Boost Gauge
	{
		rcBoostBar = rcGameplayColors->CreateScene("gauge_energy");
		CSDCommon::PlayAnimation(*rcBoostBar, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 100);
		//Create, set position, and fix boost bar glow to work with the 16:9 gameplay XNCP
		if (Configuration::IsColorsOG)
		{
			rcBoostBarGlow = rcGaugeEff->CreateScene("gauge_energy_2_eff");
			Hedgehog::Math::CVector2 posT = rcBoostBarGlow->GetNode("rt")->GetPosition();
			rcBoostBarGlow->GetNode("rt")->SetPosition(posT.x() + 5, posT.y());
			SetBoostGlowPosition(true);
		}
		else
		{
			rcBoostBarGlow = rcGameplayColors->CreateScene("gauge_energy_2_eff");
			Hedgehog::Math::CVector2 pos = rcBoostBar->GetNode("fulcrum_point_energy")->GetPosition();
			rcBoostBarGlow->GetNode("fulcrum_point_energy")->SetPosition(pos.x() - 57.8f, pos.y() + 21.13f);
			rcBoostBarGlow->SetScale(2.26f, 1);
		}

		rcWispContainer = rcGameplayColors->CreateScene("pixy", "Intro_Anim");
		rcWispContainer->m_MotionRepeatType = Chao::CSD::eMotionRepeatType_PlayOnce;
		if (isClassic)
		{
			rcBoostBar->SetHideFlag(true);
		}
	}

	rcStarRing = rcGameplayColors->CreateScene("star_ring");
	redStarCount = HudSonicStage::prevRedRingCount;
	rcWispPlume = rcGameplayColors->CreateScene("eff_plume");
	rcWispSpark = rcGameplayColors->CreateScene("eff_spark");
	rcWispEff1 = rcGameplayColors->CreateScene("eff_initial");
	/*if(!isMission)
	SetRedStarCount(GetSpecialRingCount(Common::GetCurrentStageID(), 1, redRingsMysterySource));*/
	rcSpecialRing = rcLifeColors->CreateScene("s_ring");
	rcSpecialRing->SetHideFlag(true);
	flags &= ~(0x1 | 0x2 | 0x4 | 0x200 | 0x800 | 0x200000); // Mask to prevent crash when game tries accessing the elements we disabled later on


	if (ScoreGenerationsAPI::IsAttached() && !ScoreGenerationsAPI::IsStageForbidden()) // Score
	{
		rcScoreCount = rcGameplayColors->CreateScene("info_score");
		scoreEnabled = true;
		for (size_t i = 0; i < maxScoreEntries; i++)
		{
			auto scene = rcGameplayColors->CreateScene("info_bonus");
			scene->SetHideFlag(true);
			rcScoreBonusEntries.insert(rcScoreBonusEntries.begin(), scene);			
		}
	}
	SetAllToIntroFirst();
	HudSonicStage::CreateScreen(This);
}
void StartWispBar(bool rocket)
{
	CSDCommon::FreezeMotion(*rcBoostBar);
	CSDCommon::PlayAnimation(*rcBoostBar, "gauge_time", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 500.0f);
	if (rocket)
	{
		CSDCommon::PlayAnimation(*rcBoostBar, "SetBarOrange", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0.0f);
	}
	else
	{
		CSDCommon::PlayAnimation(*rcBoostBar, "SetBarPurple", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0.0f);
	}
	CSDCommon::PlayAnimation(*rcBoostBar, "mode_change", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
}
bool CompareEntries(const ScoreEntry& a, const ScoreEntry& b)
{
	return a.frame > b.frame;
}
void ScoreManager_CheckForOutro(int currentFrame)
{
	for (size_t i = 0; i < scoreEntries.size(); i++)
	{
		if (scoreEntries[i].frame + 300 <= currentFrame)
		{
			CSDCommon::PlayAnimation(*rcScoreBonusEntries[scoreEntries[i].sceneIndex], "Outro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
			scoreEntries.erase(scoreEntries.begin() + i);
		}
		
	}
}
void ScoreManager_AddEntry(int type, int currentFrame)
{
	int clampedEntry = Common::RepeatUInt32(currentlyActiveEntries+1, 0, maxScoreEntries-1);
	ScoreEntry newEntry = ScoreEntry();
	newEntry.frame = currentFrame;
	newEntry.sceneIndex = clampedEntry;
	newEntry.type = type;
	currentlyActiveEntries++;
	currentlyActiveEntries = Common::RepeatUInt32(currentlyActiveEntries, 0, maxScoreEntries - 1);

	scoreEntries.insert(scoreEntries.begin(), newEntry);
	if (scoreEntries.size() > maxScoreEntries + 1)
		scoreEntries.resize(maxScoreEntries);
	std::sort(scoreEntries.begin(), scoreEntries.end(), CompareEntries);
	
	if (scoreEntries.size() > maxScoreEntries-1)
		scoreEntries.resize(maxScoreEntries);
	for (size_t i = 0; i < scoreEntries.size(); i++)
	{
		if (scoreEntries[i].timeout)
			continue;
		char buff[16];
		snprintf(buff, 16, "b_%d", i);
		auto bonusPos = rcScoreCount->GetNode(buff)->GetPosition();
		
		rcScoreBonusEntries[scoreEntries[i].sceneIndex]->SetPosition(bonusPos.x(), bonusPos.y());
		rcScoreBonusEntries[scoreEntries[i].sceneIndex]->SetHideFlag(false);
		//figure this out
		/*CSDCommon::PlayAnimation(*rcScoreBonusEntries[scoreEntries[i].sceneIndex], "Usual_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);*/
		if (i == 0)
		{
			CSDCommon::PlayAnimation(*rcScoreBonusEntries[scoreEntries[i].sceneIndex], "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
			//0 = drift | 1 = trick | 2 = quick
			switch (type)
			{
			case 0:
			{
				rcScoreBonusEntries[scoreEntries[i].sceneIndex]->GetNode("word_bonus")->SetPatternIndex(2);
				char buffT[8];
				snprintf(buffT, 8, "%08d", ScoreGenerationsAPI::GetScoreTable().Drift);
				rcScoreBonusEntries[scoreEntries[i].sceneIndex]->GetNode("num_bonus")->SetText(buffT);
				break;
			}
			case 1:
			{
				rcScoreBonusEntries[scoreEntries[i].sceneIndex]->GetNode("word_bonus")->SetPatternIndex(1);
				char buffT[8];
				snprintf(buffT, 8, "%08d", ScoreGenerationsAPI::GetScoreTable().Trick);
				rcScoreBonusEntries[scoreEntries[i].sceneIndex]->GetNode("num_bonus")->SetText(buffT);
				break;
			}
			case 2:
			{
				rcScoreBonusEntries[scoreEntries[i].sceneIndex]->GetNode("word_bonus")->SetPatternIndex(4);
				char buffT[8];
				snprintf(buffT, 8, "%08d", ScoreGenerationsAPI::GetScoreTable().QuickStep);
				rcScoreBonusEntries[scoreEntries[i].sceneIndex]->GetNode("num_bonus")->SetText(buffT);
				break;
			}
			}

		}

	}

}
void ScoreUpdateListener(int currentFrame)
{
	auto currentStats = ScoreGenerationsAPI::GetStatistics();

	if (currentStats.totalDrifts > prevScoreTotals.totalDrifts)
	{
		ScoreManager_AddEntry(0, currentFrame);
	}
	if (currentStats.totalTricks > prevScoreTotals.totalTricks)
	{
		ScoreManager_AddEntry(1, currentFrame);
	}
	if (currentStats.totalQuickSteps > prevScoreTotals.totalQuickSteps)
	{
		ScoreManager_AddEntry(2, currentFrame);
	}
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

	if (rcLife && gotNewLife)
	{
		rcLife->SetPosition(1280 / 2, 720 / 2);
		switch (currentLifeAnim)
		{
		case 1:
		{
			//Life intro_anim
			if (rcLife->m_MotionDisableFlag == 1) currentLifeAnim = 2;
			break;
		}
		case 2: {
			//Life 1up
			if (rcLife->m_MotionDisableFlag == 1)
			{
				currentLifeAnim = 3;
				CSDCommon::PlayAnimation(*rcLife, "Outro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, false, false);
			}
			break;
		}
		case 3:
		{
			//Life outro_anim
			if (rcLife->m_MotionDisableFlag == 1) gotNewLife = false;
			break;
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
		if (rcStarRing)
			CSDCommon::IntroAnim(rcStarRing);

		timeStarted = true;
	}
	if (timeStarted && !boostIntroPlayed && rcBoostBar)
	{
		if (rcBoostBar)
			boostIntroPlayed = rcBoostBar->m_MotionDisableFlag == 1;
		else
			boostIntroPlayed = true;
	}
	if (boostIntroPlayed && !wispBarUpdated)
	{
		if (currentSkillIndex != -1)
		{
			SetWispContainer(currentSkillIndex);
			WispBarSet(true);
			wispBarUpdated = true;
		}
		else
			wispBarUpdated = false;
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

	if (rcBoostBarGlow)
		rcBoostBarGlow->SetHideFlag(!isBoosting);


	if (rcBoostBar && rcWispContainer)
	{
		//Boosting Anims
		if (!currentlyUsingWisp)
		{
			///Boosting Anims

			if (playerContext->StateFlag(eStateFlag_Boost))
			{
				if (!isBoosting)
				{
					glowAnimState = 0;
				}
				ShakeBoostElements(in_rUpdateInfo.DeltaTime);
				isBoosting = true;
			}
			else
			{
				if (isBoosting)
				{
					if (hasWisp)
						CSDCommon::PlayAnimation(*rcWispContainer, "mode_change", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 100);
					else
						CSDCommon::PlayAnimation(*rcWispContainer, "mode_change_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 100);
				}

				/*if (glowAnimState == 2)
				{
					CSDCommon::PlayAnimation(*rcBoostBarGlow, "Outro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
				}*/
				isBoosting = false;
			}
			if (boostIntroPlayed && !currentlyUsingWisp)
				SetBoostValue(playerContext->m_ChaosEnergy);

		}
		//Wisp Conversion
		Hedgehog::Base::CSharedString stateCheck = playerContext->m_pPlayer->m_StateMachine.GetCurrentState()->GetStateName();
		std::string stateCheckS(stateCheck.c_str());
		DebugDrawText::log(stateCheckS.c_str(), 0);
		//Replace these with state flags once they're added to BlueBlur
		if ((stateCheckS.find("Rocket") != std::string::npos) && stateCheckS != "TransformRocket" || ((stateCheckS.find("Spike") != std::string::npos) && stateCheckS != "TransformSpike"))
		{
			if (!currentlyUsingWisp)
			{
				if (isClassic)
					rcBoostBar->SetHideFlag(false);
				//1 = Rocket | 0 = Spikes
				wispType = stateCheckS.find("Rocket") != std::string::npos;
				StartWispBar(wispType);
				currentlyUsingWisp = true;
			}

			ShakeBoostElements(in_rUpdateInfo.DeltaTime, true);
		}
		else if(stateCheckS != "ExternalControl") //Rocket and Spike use external controls when interacting with objects
		{
			if (currentlyUsingWisp && boostTransitionPlayed)
			{
				hasWisp = false;
				if (isClassic)
					rcBoostBar->SetHideFlag(true);

				boostTransitionPlayed = false;
				WispBarSet(false);
				CSDCommon::PlayAnimation(*rcBoostBar, "mode_change_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
			}
			if (!boostTransitionPlayed)
			{
				boostTransitionPlayed = rcBoostBar->m_MotionDisableFlag;
				currentlyUsingWisp = !boostTransitionPlayed;
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
		if(playerContext->m_RingCount == 0 && boostIntroPlayed)
			CSDCommon::PlayAnimation(*rcRingCount, "zero_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);

		prevRingCount = playerContext->m_RingCount;
	}
	if (gettingWisp)
	{
		if (rcWispContainer->m_MotionDisableFlag)
			gettingWisp = false;
	}
	if (rcWispEff1)
	{
		rcWispPlume->SetHideFlag(!currentlyUsingWisp);
		rcWispSpark->SetHideFlag(!currentlyUsingWisp);
		rcWispEff1->SetHideFlag(!currentlyUsingWisp);
	}

	if (rcSpecialRing)
	{
		rcSpecialRing->SetPosition(1280 / 2, 720 / 2);
		if (spinningRedRing)
		{
			if (animStateSRing == 1)
			{
				if (rcSpecialRing->m_MotionFrame == rcSpecialRing->m_MotionEndFrame)
					spinTimes++;
				if (spinTimes >= 3)
				{
					CSDCommon::PlayAnimation(*rcSpecialRing, "Outro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
					spinningRedRing = false;
				}
			}
			if (rcSpecialRing->m_MotionDisableFlag)
			{
				switch (animStateSRing)
				{
				case 0:
				{
					CSDCommon::PlayAnimation(*rcSpecialRing, "Usual_Anim", Chao::CSD::eMotionRepeatType_Loop, 1, 0);
					animStateSRing = 1;
					break;
				}
				case 2:
				{
					rcSpecialRing->SetHideFlag(true);
					break;
				}
				default:
					break;
				}
			}
		}
	}
	if (scoreEnabled)
	{
		ScoreUpdateListener(in_rUpdateInfo.Frame);
		ScoreManager_CheckForOutro(in_rUpdateInfo.Frame);
		prevScoreTotals = ScoreGenerationsAPI::GetStatistics();
	}

}

HOOK(void, __fastcall, MsgChangeCustomHud, 0x10947A0, Sonic::CGameObject* This, void* Edx, hh::fnd::Message& in_rMsg)
{
	size_t* spriteIndex = (size_t*)((char*)&in_rMsg + 16);
	const size_t iconTypeT = *(size_t*)((char*)&in_rMsg + 8);
	const size_t spriteCount = *(size_t*)((char*)&in_rMsg + 20);

	SetWispContainer(*spriteIndex);
	if (spriteCount < 1)
	{
		hasWisp = false;
		WispBarSet(false);
		currentSkillIndex = -1;
	}
	else
	{
		hasWisp = true;
		WispBarSet(true);
		currentSkillIndex = *spriteIndex;
	}
	currentSkillIndex = *spriteIndex;

}

HOOK(void, __fastcall, ProcMsgChangeWispHud, 0x1096050, Sonic::CGameObject* This, void* Edx, hh::fnd::Message& in_rMsg)
{
	const size_t motionType = *(size_t*)((char*)&in_rMsg + 16);
	const size_t wispType = *(size_t*)((char*)&in_rMsg + 20);
	DebugDrawText::log(std::to_string(wispType).c_str());

	if (rcWispContainer && !currentlyUsingWisp)
	{
		if (!hasWisp)
			WispBarSet(true);
		hasWisp = true;
		switch (wispType)
		{
		case 0:
		{
			//Rocket
			SetWispContainer(20);
			break;
		}
		case 1:
		{
			//Spikes
			SetWispContainer(21);
			break;
		}
		case 3:
		{
			//Knuckles
			SetWispContainer(2);
			break;
		}
		case 4:
		{
			//Amy
			SetWispContainer(3);
			break;
		}
		case 6:
		{
			//Rouge
			SetWispContainer(1);
			break;
		}
		case 8:
		{
			//Charmy
			SetWispContainer(16);
			break;
		}
		case 10:
		{
			//Blaze
			SetWispContainer(17);
			break;
		}

		}
	}
}
HOOK(void, __stdcall, CPlayerGetLifeHUD, 0xE75520, Sonic::Player::CPlayerContext* context, int lifeCount, bool playSound)
{
	/*originalCPlayerGetLife(context, lifeCount, playSound);*/

	if (lifeCount > 0)
	{
		if (rcLife)
		{
			rcLife->GetNode("icon")->SetPosition(1, 1);
			CSDCommon::PlayAnimation(*rcLife, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, false, false);
		}
		gotNewLife = true;
		currentLifeAnim = 1;
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
//_DWORD *__thiscall sub_E3DE00(void *this)
//The actual message is optimized, had to go around it somehow
HOOK(void, __fastcall, StartSpikesPlugin, 0xE3DFB0, void* This)
{
	timerSpike = 15;
	originalStartSpikesPlugin(This);
}
HOOK(char, __fastcall, ExtendSpikes, 0xE3DD00, void* This, void* Edx, int a2)
{
	timerSpike = 15;
	return originalExtendSpikes(This, Edx, a2);
}
HOOK(void, __fastcall, RocketStart, 0xE3E050, void* This)
{
	timerRocket = 3;
	originalRocketStart(This);
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
	const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();

	if (playerContext)
	{
		if (!passedCheckpoint)
			timeSinceStart = 0;
	}
	else
		timeSinceStart = 0;
	WispBarSet(false);
	boostIntroPlayed = false;
	timeStarted = false;
	wispBarUpdated = false;
	SetAllToIntroFirst();
	if (boostIntroPlayed)
		SetRedStarCount(GetSpecialRingCount(Common::GetCurrentStageID(), 0, redRingsMysterySource));
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
HOOK(void, __fastcall, Test, 0x1097640, DWORD* This, int a2, void* Edx)
{
	passedCheckpoint = true;
	originalTest(This, a2, Edx);
}
HOOK(void*, __fastcall, SetConverseCommonInfo, 0x6AFBA0, void* This, void* Edx, uint32_t* info)
{
	printf("\n%zu\n", info[3]);
	if (info[3] == 0xE2FFFFFF)
	{
		// Force black text to be white
		info[3] = 0xFFFFFFFF;
	}
	return originalSetConverseCommonInfo(This, Edx, info);
}
//DEFINITELY need to find a better way to do this.
HOOK(int, __fastcall, GetRedRingCount, 0xD591B0, int a1, int unused, DWORD* a3)
{
	if (!isMission)
	{
		redRingsMysterySource = a3;
		HudSonicStage::prevRedRingCount = HudSonicStage::redRingCount;
		HudSonicStage::redRingCount = originalGetRedRingCount(a1, unused, a3);

		if (HudSonicStage::prevRedRingCount == 0)
			HudSonicStage::prevRedRingCount = HudSonicStage::redRingCount;
	}
	return originalGetRedRingCount(a1, unused, a3);
}

HOOK(void, __fastcall, SpawnRedRingEffect, 0x11A9BA0, void* This, int a2, void* Edx)
{

	CSDCommon::PlayAnimation(*rcSpecialRing, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, false, false);
	spinTimes = 0;
	animStateSRing = 0;
	spinningRedRing = true;
	rcSpecialRing->SetHideFlag(false);

	SetRedStarCount(redStarCount + 1);
	return originalSpawnRedRingEffect(This, a2, Edx);
}

void __declspec(naked) GetSkilLThingA()
{
	static uint32_t sub_446E30 = 0x446E30;
	static uint32_t returnAddress = 0x01097130;
	static int skill;
	__asm
	{
		mov     eax, [esp + 40h - 24h]
		jmp[returnAddress]
	}
}
void HudSonicStage::Install()
{
	INSTALL_HOOK(CHudSonicStageUpdate);
	INSTALL_HOOK(MsgStartMode);
	INSTALL_HOOK(MsgRestartStage);
	//INSTALL_HOOK(GetRedRingCount);
	INSTALL_HOOK(Test);
	INSTALL_HOOK(SpawnRedRingEffect);
	INSTALL_HOOK(StartSpikesPlugin);
	INSTALL_HOOK(ExtendSpikes);
	INSTALL_HOOK(RocketStart);
	//WRITE_JUMP(0x01097129, GetSkilLThingA)
	/*INSTALL_HOOK(TestRestart);*/

	//INSTALL_HOOK(SetConverseCommonInfo);
	INSTALL_HOOK(CHudSonicStageDelayProcessImp);
	INSTALL_HOOK(CHudSonicStageUpdateParallel);
	INSTALL_HOOK(MsgChangeCustomHud);
	INSTALL_HOOK(ProcMsgChangeWispHud);
	INSTALL_HOOK(CPlayerGetLifeHUD);
	WRITE_MEMORY(0x16A467C, void*, CHudSonicStageRemoveCallback);

	WRITE_MEMORY(0x109B1A4, uint8_t, 0xE9, 0xDC, 0x02, 0x00, 0x00); // Disable lives
	WRITE_MEMORY(0X0109D328, uint8_t, 0x90, 0xE9); // Disable time
	WRITE_MEMORY(0x109B490, uint8_t, 0x90, 0xE9); // Disable time
	WRITE_MEMORY(0x109B5AD, uint8_t, 0x90, 0xE9); // Disable rings
	WRITE_MEMORY(0x109B8F5, uint8_t, 0x90, 0xE9); // Disable boost gauge
	WRITE_MEMORY(0x109BC88, uint8_t, 0x90, 0xE9); // Disable boost button
	WRITE_MEMORY(0x109B6A7, uint8_t, 0x90, 0xE9); // Disable final boss rings


	WRITE_MEMORY(0xCFED0E, uint32_t, 4);
	WRITE_JUMP(0xCFEC3D, (void*)0xCFEC94);
	WRITE_JUMP(0xCFECB4, (void*)0xCFED0B);
	WRITE_JUMP(0xCFEDC7, (void*)0xCFE888);
	WRITE_MEMORY(0xCFE6FD, uint8_t, 0xEB); // Don't play Game Over ticking sfx
	INSTALL_HOOK(MsgStartCommonButtonSign);
	// Patch ring counter to use four digits.
	WRITE_MEMORY(0x168D33C, const char, "%04d");

	WRITE_MEMORY(0x168E8E0, const char, "%04d");
}