boost::shared_ptr<Sonic::CGameObjectCSD> spResult, spBtnGuide, spCountdownTimer;
Chao::CSD::RCPtr<Chao::CSD::CProject> rcProjectResult, rcProjectBtnGuide, rcProjectCountdownTimer;
Chao::CSD::RCPtr<Chao::CSD::CScene> rcResultTitle;
Chao::CSD::RCPtr<Chao::CSD::CScene> rcResultSideCounters;
Chao::CSD::RCPtr<Chao::CSD::CScene> rcResultRankSprite;
Chao::CSD::RCPtr<Chao::CSD::CScene> rcResultScoreNum[8];
Chao::CSD::RCPtr<Chao::CSD::CScene> rcResultRankScore;
Chao::CSD::RCPtr<Chao::CSD::CScene> rcBtnGuide;
Chao::CSD::RCPtr<Chao::CSD::CScene> rcCountdownTimer;

float m_resultTimer = 0.0f;
HudResult::ResultState m_resultState = HudResult::ResultState::Idle;
HudResult::ResultState m_resultStateNew = HudResult::ResultState::Idle;

float m_enemyBonus = 0.0f;
float m_enemyChainBonus = 0.0f;
float m_trickBonus = 0.0f;
float m_trickBonusLimit = 0.0f;
float m_trickChainMultiplier = 0.0f;
float m_trickFinishBonus = 0.0f;
float m_rainbowRingBonus = 0.0f;
bool m_ScoreEnabled = false;

HudResult::SideState counterState = (HudResult::SideState)0;
HudResult::ScoreState scoreState = (HudResult::ScoreState)0;
//Timers
float clickSoundTimer, scoreSpinTimer, delayRingAnim = 0;
//Timer Toggles
bool startClick, playedTick, delayRing = false;
float m_stageTime = 0.0f;
HudResult::ResultData m_resultData;
HudResult::StageData m_stageData;
bool m_isEnemyChain = false;
int m_trickChain = 0;
static SharedPtrTypeless tickSoundEffect;

HOOK(void, __fastcall, HudResult_MsgStartGoalResult, 0x10B58A0, uint32_t* This, void* Edx, void* message)
{
	// Don't run the message itself
}

void __fastcall HudResult_CHudResultRemoveCallback(Sonic::CGameObject* This, void*, Sonic::CGameDocument* pGameDocument)
{
	if (spResult)
	{
		spResult->SendMessage(spResult->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
		spResult = nullptr;
	}
	if (spBtnGuide)
	{
		spBtnGuide->SendMessage(spBtnGuide->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
		spBtnGuide = nullptr;
	}

	Chao::CSD::CProject::DestroyScene(rcProjectResult.Get(), rcResultTitle);
	Chao::CSD::CProject::DestroyScene(rcProjectResult.Get(), rcResultSideCounters);
	Chao::CSD::CProject::DestroyScene(rcProjectResult.Get(), rcResultRankScore);
	Chao::CSD::CProject::DestroyScene(rcProjectResult.Get(), rcResultRankSprite);
	Chao::CSD::CProject::DestroyScene(rcProjectBtnGuide.Get(), rcBtnGuide);
	Chao::CSD::CProject::DestroyScene(rcProjectCountdownTimer.Get(), rcCountdownTimer);
	tickSoundEffect.reset();
	for (size_t i = 0; i < 8; i++)
	{
		Chao::CSD::CProject::DestroyScene(rcProjectResult.Get(), rcResultScoreNum[i]);
	}startClick, playedTick, delayRing = false;
	clickSoundTimer, scoreSpinTimer, delayRingAnim = 0;
	clickSoundTimer = 0;
	scoreState = HudResult::ScoreIdle;
	counterState = HudResult::WaitingForTime;
	rcProjectResult = nullptr;
}

HOOK(int, __fastcall, HudResult_CStateGoalFadeBeforeBegin, 0xCFE080, uint32_t* This)
{
	int result = originalHudResult_CStateGoalFadeBeforeBegin(This);
	{
		m_resultTimer = 0.0f;
		m_resultState = HudResult::ResultState::Idle;
		m_resultStateNew = HudResult::ResultState::Idle;

		m_stageTime = *(float*)Common::GetMultiLevelAddress(This[2] + 0x60, { 0x8, 0x184 });
		m_resultData = *(HudResult::ResultData*)(This[2] + 0x16C);

		// Calculate scores
		uint32_t ringCount = Sonic::Player::CPlayerSpeedContext::GetInstance()->m_RingCount;
		m_stageData.m_ringScore = ringCount * 100.0f;
		m_ScoreEnabled = ScoreGenerationsAPI::IsAttached();
		if (!m_ScoreEnabled)
		{
			// Set to Generations ring score if score is not enabled
			m_stageData.m_ringScore = min(5000, ringCount * 50);
		}
		else if (GetModuleHandle(TEXT("ScoreGenerations.dll")) != nullptr)
		{
			// Get speed score from Score Generations
			m_stageData.m_speedScore = ScoreGenerationsAPI::ComputeSpeedBonus();
		}
	}
	return result;
}

HOOK(void, __fastcall, HudResult_CStateGoalFadeInEnd, 0xCFA470, hh::fnd::CStateMachineBase::CStateBase* This)
{
	originalHudResult_CStateGoalFadeInEnd(This);

	// Result starts
	if (This->m_Time > *(float*)0x1A426B8)
	{
		m_resultTimer = 0.0f;
		m_resultStateNew = HudResult::ResultState::MainWait;
	}
}
void SetScoreText(const char* number)
{
	//Yeah i know this is a bit spaghet, but the numbers are sprites.
	int number1 = (int)number[0] - 48;
	int number2 = (int)number[1] - 48;
	int number3 = (int)number[2] - 48;
	int number4 = (int)number[3] - 48;
	int number5 = (int)number[4] - 48;
	int number6 = (int)number[5] - 48;
	int number7 = (int)number[6] - 48;
	int number8 = (int)number[7] - 48;
#pragma region num_score_frame
	rcResultRankScore->GetNode("num_b_1")->SetPatternIndex(number1);
	rcResultRankScore->GetNode("num_b_2")->SetPatternIndex(number2);
	rcResultRankScore->GetNode("num_b_3")->SetPatternIndex(number3);
	rcResultRankScore->GetNode("num_b_4")->SetPatternIndex(number4);
	rcResultRankScore->GetNode("num_b_5")->SetPatternIndex(number5);
	rcResultRankScore->GetNode("num_b_6")->SetPatternIndex(number6);
	rcResultRankScore->GetNode("num_b_7")->SetPatternIndex(number7);
	rcResultRankScore->GetNode("num_b_8")->SetPatternIndex(number8);
#pragma endregion
#pragma region num_score_bg_brilliance
	rcResultRankScore->GetNode("num_d_1")->SetPatternIndex(number1);
	rcResultRankScore->GetNode("num_d_2")->SetPatternIndex(number2);
	rcResultRankScore->GetNode("num_d_3")->SetPatternIndex(number3);
	rcResultRankScore->GetNode("num_d_4")->SetPatternIndex(number4);
	rcResultRankScore->GetNode("num_d_5")->SetPatternIndex(number5);
	rcResultRankScore->GetNode("num_d_6")->SetPatternIndex(number6);
	rcResultRankScore->GetNode("num_d_7")->SetPatternIndex(number7);
	rcResultRankScore->GetNode("num_d_8")->SetPatternIndex(number8);
#pragma endregion
#pragma region num_score_brilliance
	rcResultRankScore->GetNode("num_a_1")->SetPatternIndex(number1);
	rcResultRankScore->GetNode("num_a_2")->SetPatternIndex(number2);
	rcResultRankScore->GetNode("num_a_3")->SetPatternIndex(number3);
	rcResultRankScore->GetNode("num_a_4")->SetPatternIndex(number4);
	rcResultRankScore->GetNode("num_a_5")->SetPatternIndex(number5);
	rcResultRankScore->GetNode("num_a_6")->SetPatternIndex(number6);
	rcResultRankScore->GetNode("num_a_7")->SetPatternIndex(number7);
	rcResultRankScore->GetNode("num_a_8")->SetPatternIndex(number8);
#pragma endregion	
#pragma region scorenum

	rcResultScoreNum[0]->GetNode("num_c")->SetPatternIndex(number1);
	rcResultScoreNum[0]->GetNode("num_c_brilliance")->SetPatternIndex(number1);

	rcResultScoreNum[1]->GetNode("num_c")->SetPatternIndex(number2);
	rcResultScoreNum[1]->GetNode("num_c_brilliance")->SetPatternIndex(number2);

	rcResultScoreNum[2]->GetNode("num_c")->SetPatternIndex(number3);
	rcResultScoreNum[2]->GetNode("num_c_brilliance")->SetPatternIndex(number3);

	rcResultScoreNum[3]->GetNode("num_c")->SetPatternIndex(number4);
	rcResultScoreNum[3]->GetNode("num_c_brilliance")->SetPatternIndex(number4);

	rcResultScoreNum[4]->GetNode("num_c")->SetPatternIndex(number5);
	rcResultScoreNum[4]->GetNode("num_c_brilliance")->SetPatternIndex(number5);

	rcResultScoreNum[5]->GetNode("num_c")->SetPatternIndex(number6);
	rcResultScoreNum[5]->GetNode("num_c_brilliance")->SetPatternIndex(number6);

	rcResultScoreNum[6]->GetNode("num_c")->SetPatternIndex(number7);
	rcResultScoreNum[6]->GetNode("num_c_brilliance")->SetPatternIndex(number7);

	rcResultScoreNum[7]->GetNode("num_c")->SetPatternIndex(number8);
	rcResultScoreNum[7]->GetNode("num_c_brilliance")->SetPatternIndex(number8);

	//rcResultScoreNum[8]->GetNode("num_c")->SetPatternIndex(number8);
	//rcResultScoreNum[8]->GetNode("num_c_brilliance")->SetPatternIndex(number8);
#pragma endregion
}
HOOK(int, __fastcall, HudResult_CHudResultAddCallback, 0x10B8ED0, Sonic::CGameObject* This, void* Edx, int a2, int a3, int a4)
{
	int result = originalHudResult_CHudResultAddCallback(This, Edx, a2, a3, a4);
	HudResult_CHudResultRemoveCallback(This, nullptr, nullptr);

	printf("[Unleashed HUD] Result created\n");
	Sonic::CCsdDatabaseWrapper wrapper(This->m_pMember->m_pGameDocument->m_pMember->m_spDatabase.get());

	auto spCsdProject = wrapper.GetCsdProject("ui_result_colors");
	rcProjectResult = spCsdProject->m_rcProject;
	spCsdProject = wrapper.GetCsdProject("ui_btn_guide_colors");
	rcProjectBtnGuide = spCsdProject->m_rcProject;
	spCsdProject = wrapper.GetCsdProject("ui_burndown-timer");
	rcProjectCountdownTimer = spCsdProject->m_rcProject;

	rcResultTitle = rcProjectResult->CreateScene("upper");
	rcResultRankScore = rcProjectResult->CreateScene("result");
	rcResultSideCounters = rcProjectResult->CreateScene("stage_time_ring");

	rcBtnGuide = rcProjectBtnGuide->CreateScene("btn_guide");
	rcBtnGuide->GetNode("position_2")->SetHideFlag(true);
	rcBtnGuide->GetNode("btn_1")->SetHideFlag(true);
	rcBtnGuide->GetNode("btn_2")->SetHideFlag(true);
	rcBtnGuide->GetNode("btn_3")->SetPatternIndex(12);
	rcBtnGuide->GetNode("btn_4")->SetPatternIndex(11);
	rcBtnGuide->GetNode("word_3")->SetPatternIndex(4);
	rcBtnGuide->GetNode("word_4")->SetPatternIndex(3);
	rcBtnGuide->GetNode("word_3")->SetScale(0.75f, 1);
	rcBtnGuide->GetNode("word_4")->SetScale(0.75f, 1);

	CSDCommon::FreezeMotion(*rcResultTitle, 0);
	CSDCommon::FreezeMotion(*rcResultRankScore);
	CSDCommon::FreezeMotion(*rcResultSideCounters);
	rcResultRankScore->SetHideFlag(true);
	rcResultSideCounters->SetHideFlag(true);
	rcResultRankSprite = rcProjectResult->CreateScene("word_rank");
	rcResultRankSprite->SetHideFlag(true);
	for (size_t i = 0; i < 8; i++)
	{
		rcResultScoreNum[i] = rcProjectResult->CreateScene("num_score");
		CSDCommon::PlayAnimation(*rcResultScoreNum[i], "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		rcResultScoreNum[i]->SetPosition(-100, 0);
		rcResultScoreNum[i]->SetScale(2.5f, 2.5f);
		rcResultScoreNum[i]->SetHideFlag(true);

	}

	rcCountdownTimer = rcProjectCountdownTimer->CreateScene("Test_timer");
	CSDCommon::FreezeMotion(*rcCountdownTimer, 0);
	
	if (rcProjectResult && !spResult)
	{
		spResult = boost::make_shared<Sonic::CGameObjectCSD>(rcProjectResult, 0.5f, "HUD", false);
		Sonic::CGameDocument::GetInstance()->AddGameObject(spResult, "main", This);

	}
	if (rcProjectBtnGuide && !spBtnGuide)
	{
		spBtnGuide = boost::make_shared<Sonic::CGameObjectCSD>(rcProjectBtnGuide, 0.5f, "HUD", false);
		Sonic::CGameDocument::GetInstance()->AddGameObject(spBtnGuide, "main", This);
	}
	if (rcProjectCountdownTimer && !spCountdownTimer)
	{
		spCountdownTimer = boost::make_shared<Sonic::CGameObjectCSD>(rcProjectCountdownTimer, 0.5f, "HUD", false);
		Sonic::CGameDocument::GetInstance()->AddGameObject(spCountdownTimer, "main", This);
	}

	return result;
}

void HudResult_PlayMotion(Chao::CSD::RCPtr<Chao::CSD::CScene>& scene, char const* motion, bool loop = false)
{
	if (!scene) return;
	scene->SetHideFlag(false);
	scene->SetMotion(motion);
	scene->SetMotionFrame(0.0f);
	scene->m_MotionDisableFlag = false;
	scene->m_MotionSpeed = 1.0f;
	scene->m_MotionRepeatType = loop ? Chao::CSD::eMotionRepeatType_Loop : Chao::CSD::eMotionRepeatType_PlayOnce;
	scene->Update();
}

float const cResultMainDelay = 0.2833f;
float const cResultNewRecordDelay = 1.6667f;
float const cResultRankDelay = 0.9f;
float const cResultFooterDelay = 2.0f;
bool hasNewRecord = false;
HOOK(void, __fastcall, HudResult_CHudResultAdvance, 0x10B96D0, Sonic::CGameObject* This, void* Edx, const hh::fnd::SUpdateInfo& in_rUpdateInfo)
{
	if (HudLoading::IsFadeOutCompleted())
	{
		originalHudResult_CHudResultAdvance(This, Edx, in_rUpdateInfo);
		if (*(uint32_t*)0x10B96E6 != 0xFFD285E8)
		{
			// We are finished
			HudResult_CHudResultRemoveCallback(This, nullptr, nullptr);
			WRITE_MEMORY(0x10B96E6, uint8_t, 0xE8, 0x85, 0xD2, 0xFF, 0xFF);
			WRITE_MEMORY(0x10B9A7C, uint8_t, 0xE8, 0xCF, 0x7A, 0x5A, 0xFF);
			WRITE_MEMORY(0x10B9976, uint8_t, 0xE8, 0xD5, 0x7B, 0x5A, 0xFF);
			return;
		}
	}

	FUNCTION_PTR(bool, __cdecl, IsFirstTimePlayStage, 0x10B7BB0);

	char const* intro = "Intro_Anim";
	// New states
	if (m_resultState != m_resultStateNew)
	{
		m_resultState = m_resultStateNew;
		switch (m_resultState)
		{
		case HudResult::ResultState::Idle:
		{
			break;
		}
		case HudResult::ResultState::MainWait:
		{
			break;
		}
		case HudResult::ResultState::Main:
		{

			rcResultTitle->SetHideFlag(false);
			rcResultRankScore->SetHideFlag(false);
			rcResultSideCounters->SetHideFlag(false);
			rcResultRankSprite->SetHideFlag(false);
			CSDCommon::PlayAnimation(*rcResultTitle, intro, Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
			CSDCommon::PlayAnimation(*rcResultRankScore, intro, Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);

			CSDCommon::PlayAnimation(*rcResultRankSprite, intro, Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
			for (size_t i = 0; i < 8; i++)
			{
				static char name[16];
				sprintf(name, "num_c_%d", i + 1);

				Hedgehog::Math::CVector2 posscore = rcResultRankScore->GetNode(name)->GetPosition();

				CSDCommon::PlayAnimation(*rcResultScoreNum[i], intro, Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
				rcResultScoreNum[i]->SetHideFlag(false);
				rcResultScoreNum[i]->SetPosition(posscore.x(), posscore.y());
				rcResultScoreNum[i]->SetScale(2.5f, 2.5f);

			}
			rcResultRankSprite->SetScale(2.5f, 2.5f);
			Hedgehog::Math::CVector2 posrank = rcResultRankScore->GetNode("rank_bg_b")->GetPosition();

			rcResultRankSprite->SetPosition(posrank.x(), posrank.y());
			int millisecond = (int)(m_stageTime * 100.0f) % 100;
			int second = (int)m_stageTime % 60;
			int minute = (int)m_stageTime / 60;

			static char time[16];
			sprintf(time, "%02d:%02d.%02d", minute, second, millisecond);
			static char ring[16];
			sprintf(ring, "%04d", Sonic::Player::CPlayerSpeedContext::GetInstance()->m_RingCount);

			static char score[16];
			if (!m_ScoreEnabled)
			{
				sprintf(score, "%08d", m_resultData.m_score);
			}
			else
			{
				sprintf(score, "%08d", ScoreGenerationsAPI::GetScore());
			}
			rcResultSideCounters->GetNode("num_time")->SetText(time);
			rcResultSideCounters->GetNode("num_time_shade")->SetText(time);
			CSDCommon::PlayAnimation(*rcResultSideCounters, "Intro_Anim_time", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
			static SharedPtrTypeless soundHandle;

			Common::PlaySoundStatic(soundHandle, 1010005);
			Common::PlaySoundStatic(soundHandle, 1010002);
			Common::PlaySoundStatic(soundHandle, 1000000);

			startClick = true;
			clickSoundTimer = 0;
			rcResultSideCounters->GetNode("num_ring")->SetText(ring);
			rcResultSideCounters->GetNode("num_ring_shade")->SetText(ring);
			SetScoreText(score);



			printf("[Unleashed HUD] Result State: New Record\n");

			hasNewRecord = false;
			constexpr float newRecordX = 0.628125f * 1280.0f;

			//// New record data, based on sub_10B7F00
			//rcResultNewRecordTime = rcProjectResult->CreateScene("result_newR");
			//if (((1 << (0 & 0x1F)) & *(uint32_t*)((uint32_t)This + 580 + 4 * (0 >> 5))) != 0)
			//{
			//	rcResultNewRecordTime->SetPosition(newRecordX, +0.32083333f * 720.0f);
			//	HudResult_PlayMotion(rcResultNewRecordTime, intro);
			//	hasNewRecord = true;
			//}
			//else
			//{
			//	rcResultNewRecordTime->SetHideFlag(true);
			//}

			//rcResultNewRecordScore = rcProjectResult->CreateScene("result_newR");
			//if (((1 << (2 & 0x1F)) & *(uint32_t*)((uint32_t)This + 580 + 4 * (2 >> 5))) != 0)
			//{
			//	rcResultNewRecordScore->SetPosition(newRecordX, +0.7861111f * 720.0f);
			//	HudResult_PlayMotion(rcResultNewRecordScore, intro);
			//	hasNewRecord = true;
			//}
			//else
			//{
			//	rcResultNewRecordScore->SetHideFlag(true);
			//}


			scoreSpinTimer = 00;
			printf("[Unleashed HUD] Result State: Rank\n");

			HudResult_PlayMotion(rcResultRankScore, "Intro_Anim");

			int rankSceneName;
			switch (m_resultData.m_perfectRank)
			{
			case HudResult::ResultRankType::S: rankSceneName = 0; break;
			case HudResult::ResultRankType::A: rankSceneName = 1; break;
			case HudResult::ResultRankType::B: rankSceneName = 2; break;
			case HudResult::ResultRankType::C: rankSceneName = 3; break;
			case HudResult::ResultRankType::D: rankSceneName = 4; break;
			}

			rcResultRankScore->GetNode("rank_bg")->SetPatternIndex(rankSceneName);
			rcResultRankSprite->GetNode("img_rank")->SetPatternIndex(rankSceneName);
			rcResultRankSprite->GetNode("img_rank_brilliance")->SetPatternIndex(rankSceneName);
			HudResult_PlayMotion(rcResultRankScore, "Intro_Anim");


			printf("[Unleashed HUD] Result State: Footer\n");



			m_resultTimer = 0.0f;
		}

		// Loop animation for rank S,A,B
		if (rcResultRankScore && rcResultRankScore->m_MotionDisableFlag &&
			(m_resultData.m_perfectRank == HudResult::ResultRankType::S || m_resultData.m_perfectRank == HudResult::ResultRankType::A || m_resultData.m_perfectRank == HudResult::ResultRankType::B))
		{
			HudResult_PlayMotion(rcResultRankScore, "Usual_Anim", true);
		}
		break;
		}
	}
	m_resultTimer += in_rUpdateInfo.DeltaTime;

	//// Total/New record sfx
	//if (m_resultState == HudResult::ResultState::NewRecord)
	//{
	//	if (m_resultTimer >= 0.0667f && !m_soundState.m_total)
	//	{
	//		m_soundState.m_total = true;

	//		if (hasNewRecord)
	//		{
	//			static SharedPtrTypeless newRecordSoundHandle;
	//			Common::PlaySoundStatic(newRecordSoundHandle, 1000053);
	//		}

	//		static SharedPtrTypeless totalSoundHandle;
	//		Common::PlaySoundStatic(totalSoundHandle, 1000021);
	//	}
	//}

	//// Rank slam/rank quote sfx
	//if (m_resultState == HudResult::ResultState::Rank)
	//{
	//	if (m_resultTimer >= 0.1667f && !m_soundState.m_rank)
	//	{
	//		m_soundState.m_rank = true;

	//		uint32_t cueID;
	//		switch (m_resultData.m_perfectRank)
	//		{
	//		case HudResult::ResultRankType::S: cueID = 1000041; break;
	//		case HudResult::ResultRankType::A: cueID = 1000042; break;
	//		case HudResult::ResultRankType::B: cueID = 1000043; break;
	//		case HudResult::ResultRankType::C: cueID = 1000044; break;
	//		case HudResult::ResultRankType::D: cueID = 1000045; break;
	//		default: cueID = 1000046; break;
	//		}

	//		static SharedPtrTypeless rankSoundHandle;
	//		Common::PlaySoundStatic(rankSoundHandle, cueID);
	//	}

	//	if (!Common::IsPlayerSuper() && m_resultTimer >= 0.6833f && !m_soundState.m_rankVoice)
	//	{
	//		m_soundState.m_rankVoice = true;

	//		uint32_t cueID;
	//		switch (m_resultData.m_perfectRank)
	//		{
	//		case HudResult::ResultRankType::S: cueID = 40000; break;
	//		case HudResult::ResultRankType::A: cueID = 40001; break;
	//		case HudResult::ResultRankType::B: cueID = 40002; break;
	//		case HudResult::ResultRankType::C: cueID = 40003; break;
	//		case HudResult::ResultRankType::D: cueID = 40004; break;
	//		default: cueID = 40005; break;
	//		}

	//		static SharedPtrTypeless rankVoiceHandle;
	//		Common::PlaySoundStatic(rankVoiceHandle, cueID);
	//	}
	//}

	// State transition
	switch (m_resultState)
	{
	case HudResult::ResultState::Idle: break;
	case HudResult::ResultState::MainWait:
	{
		if (m_resultTimer > cResultMainDelay)
		{
			m_resultStateNew = HudResult::ResultState::Main;
		}
		break;
	}
	case HudResult::ResultState::Main:
	{
		if (m_resultTimer > cResultNewRecordDelay)
		{
			m_resultStateNew = HudResult::ResultState::NewRecord;
		}
		break;
	}
	case HudResult::ResultState::NewRecord:
	{
		if (m_resultTimer > cResultRankDelay)
		{
			m_resultStateNew = HudResult::ResultState::Rank;
		}
		break;
	}
	case HudResult::ResultState::Rank:
	{
		if (m_resultTimer > cResultFooterDelay)
		{
			m_resultStateNew = HudResult::ResultState::Footer;
		}
		break;
	}
	case HudResult::ResultState::Footer:
	{
		if (*(uint32_t*)0x10B96E6 == 0xFFD285E8)
		{
			static SharedPtrTypeless soundHandle;
			Sonic::SPadState const* padState = &Sonic::CInputState::GetInstance()->GetPadState();
			if (padState->IsTapped(Sonic::EKeyState::eKeyState_A))
			{
				WRITE_JUMP(0x10B96E6, (void*)0x10B974B);
				WRITE_NOP(0x10B9976, 5);
				Common::PlaySoundStatic(soundHandle, 1000005);

				HudLoading::StartFadeOut();
			}
			else if (padState->IsTapped(Sonic::EKeyState::eKeyState_Y))
			{
				WRITE_JUMP(0x10B96E6, (void*)0x10B999F);
				WRITE_NOP(0x10B9A7C, 5);
				HudLoading::StartFadeOut();
				Common::PlaySoundStatic(soundHandle, 1000005);
			}
		}

		break;
	}
	case HudResult::ResultState::FadeOut: break;
	}
}

void DebugCombinedScore()
{
	printf("[Unleashed HUD] Score: %.f\n", m_stageData.m_enemyScore + m_stageData.m_trickScore);
}

HOOK(int, __fastcall, HudResult_MsgRestartStage, 0xE76810, uint32_t* This, void* Edx, void* message)
{
	m_stageData = HudResult::StageData();
	return originalHudResult_MsgRestartStage(This, Edx, message);
}


#pragma region Hyper

// https://github.com/ActualMandM/DllMods/blob/master/Source/CustomizableResultsMusic/Mod.cpp#L163
__declspec(naked) void ResultTimer_MidAsmHook()
{
	static void* returnAddress = (void*)0x00CFD566;
	static double timer = 8.56;

	__asm
	{
		push eax
		lea eax, timer
		movsd xmm1, [eax]
		pop eax
		jmp[returnAddress]
	}
}

// https://github.com/brianuuu/DllMods/blob/master/Source/Sonic06HUD/ResultUI.cpp#L38
HOOK(int, __fastcall, CStateGoalFadeBefore, 0xCFE080, uint32_t* thisDeclaration)
{
	/* Adjust the results camera position so Sonic appears to
	   be sitting on top of the bottom UI element like Colours. */

	if (!StringHelper::IsEmpty(Configuration::UIType.c_str()))
	{
		WRITE_MEMORY(0x1A48C7C, float, -0.2f);   // X Position
		WRITE_MEMORY(0x1A48C80, float, 1.25f);   // Y Position
		WRITE_MEMORY(0x1A48C84, float, 9);       // Z Position
		WRITE_MEMORY(0x1A48C88, float, 2);       // X Rotation
		WRITE_MEMORY(0x1A48C8C, float, -10.25f); // Y Rotation
	}

	return originalCStateGoalFadeBefore(thisDeclaration);
}

#pragma endregion
HOOK(void*, __fastcall, HudResult_UpdateApplication, 0xE7BED0, void* This, void* Edx, float elapsedTime, uint8_t a3)
{

	printf("\nScoreSpinTimer: %f", scoreSpinTimer);
	printf(" | DelayRingTimer: %f", delayRingAnim);
	#pragma region Score Numbers

	if (rcResultRankScore && rcResultSideCounters)
	{
		if (scoreState == HudResult::ScoreRandomize) //Randomize Score Count
		{
			if (!playedTick)
			{
				Common::PlaySoundStatic(tickSoundEffect, 1010003);
				playedTick = true;
			}

			static char score[16];
			if (((delayRingAnim >= 5 && scoreSpinTimer == 75) || delayRingAnim < 5 && rcResultSideCounters->m_MotionFrame == 55) /*&& rcResultSideCounters->m_MotionDisableFlag*/)
			{

				tickSoundEffect.reset();
				Common::PlaySoundStatic(tickSoundEffect, 1010000);
				scoreSpinTimer += 10;
				playedTick = false;
				if (!m_ScoreEnabled)
				{
					sprintf(score, "%08d", m_resultData.m_score);
				}
				else
				{
					sprintf(score, "%08d", ScoreGenerationsAPI::GetScore());
				}
				SetScoreText(score);
				for (size_t i = 0; i < 8; i++)
				{
					CSDCommon::PlayAnimation(*rcResultScoreNum[i], "countup_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
				}
				scoreState = HudResult::ScoreIdle;
			}
			else
			{
				sprintf(score, "%08d", 1 + (int)(100000000.0 * (rand() / (RAND_MAX + 1.0))));
				SetScoreText(score);
				scoreSpinTimer += 1;
			}
			printf("\n%f\n", scoreSpinTimer);
		}
	}
#pragma endregion
	#pragma region Side Counters

	if (rcResultSideCounters) {

		static SharedPtrTypeless soundHandle;
		if (rcResultSideCounters->m_MotionFrame == 25 && counterState == HudResult::WaitingForTime) //Start spinning animation at the 25th frame in the counter
		{
			scoreState = HudResult::ScoreRandomize;
			scoreSpinTimer = 0;
		}
		if (rcResultSideCounters->m_MotionDisableFlag && counterState == HudResult::WaitingForTime) //Set variables when the first animation is done
		{
			counterState = HudResult::WaitingForRings;
			delayRing = true;
			clickSoundTimer = 0;
			scoreSpinTimer = 0;
		}

		if (delayRing) //Delay for ring count
		{
			if (delayRingAnim == 25)
			{
				Common::PlaySoundStatic(soundHandle, 1000000);
				CSDCommon::PlayAnimation(*rcResultSideCounters, "Intro_Anim_ring", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
			}
			if (delayRingAnim >= 35) //Play sound with delay to match colors
			{
				clickSoundTimer = 0;
				Common::PlaySoundStatic(soundHandle, 1010005);
				Common::PlaySoundStatic(soundHandle, 1010002);
				startClick = true;
				scoreSpinTimer = 0;
				scoreState = HudResult::ScoreRandomize;
				delayRing = false;
				counterState = HudResult::PlayEndSound;
			}
			delayRingAnim++;
		}
		//Colors (for some reason) plays the sound twice with a small delay, this tries to replicate that.
		if (clickSoundTimer >= 5 && startClick)
		{
			Common::PlaySoundStatic(soundHandle, 1010002);
			startClick = false;
		}
		else
			clickSoundTimer += 1;

		if (counterState == HudResult::PlayEndSound && rcResultSideCounters->m_MotionDisableFlag)
		{
			counterState = HudResult::Finished;
			CSDCommon::PlayAnimation(*rcCountdownTimer, "burndown_timer", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
			Common::PlaySoundStatic(soundHandle, 1010004);
		}
	}
#pragma endregion
	return originalHudResult_UpdateApplication(This, Edx, elapsedTime, a3);
}

void HudResult::Install()
{
	// Get score multiplier so we can track them
	INIReader reader("ScoreGenerations.ini");
	m_enemyBonus = reader.GetFloat("Score", "Enemy", 300.0f);
	m_enemyChainBonus = reader.GetFloat("GameplayBonus", "homingChainBonus", 200.0f);
	m_trickBonus = reader.GetFloat("Score", "Trick", 100.0f);
	m_trickBonusLimit = reader.GetFloat("GameplayBonus", "trickBonusLimit", 400.0f);
	m_trickChainMultiplier = reader.GetFloat("Multiplier", "trickMultiplier", 2.0f);
	m_trickFinishBonus = reader.GetFloat("Score", "TrickFinish", 2000.0f);
	m_rainbowRingBonus = reader.GetFloat("Score", "RainbowRing", 1000.0f);


	// Prevent using Gen's result
	INSTALL_HOOK(HudResult_MsgStartGoalResult);

	// Data collection
	INSTALL_HOOK(HudResult_CStateGoalFadeBeforeBegin);
	INSTALL_HOOK(HudResult_CStateGoalFadeInEnd);
	INSTALL_HOOK(HudResult_UpdateApplication);

	//INSTALL_HOOK(HudResult_CStateGoalFadeInBegin);

	// Result HUD handling
	INSTALL_HOOK(HudResult_CHudResultAddCallback);
	WRITE_MEMORY(0x16A1C38, void*, HudResult_CHudResultRemoveCallback);
	INSTALL_HOOK(HudResult_CHudResultAdvance);

	// Record score stats (only for cosmetics if Score Generations isn't enabled)
	INSTALL_HOOK(HudResult_MsgRestartStage);

	// Do not modify the results timer if Customizable Results Music is enabled.
	if (GetModuleHandle(TEXT("CustomizableResultsMusic.dll")) == nullptr)
		WRITE_JUMP(0xCFD55E, &ResultTimer_MidAsmHook);

	/*if (*(uint8_t*)0x15EFE9D == 0x45)
	{
		MessageBoxA(0, "This mod is not compatible with E-Rank Generations! Please disable it.", "Ultimate Colors HUD", 0);
		exit(0);
	}*/

	INSTALL_HOOK(CStateGoalFadeBefore);
}