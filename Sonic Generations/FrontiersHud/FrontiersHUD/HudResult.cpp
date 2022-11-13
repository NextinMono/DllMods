boost::shared_ptr<Sonic::CGameObjectCSD> spResult;
Chao::CSD::RCPtr<Chao::CSD::CProject> rcProjectResult;
Chao::CSD::RCPtr<Chao::CSD::CScene> rcResultTitle;

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
void __fastcall HudResult_CHudResultRemoveCallback(Sonic::CGameObject* This, void*, Sonic::CGameDocument* pGameDocument)
{
	if (spResult)
	{
		spResult->SendMessage(spResult->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
		spResult = nullptr;
	}

	Chao::CSD::CProject::DestroyScene(rcProjectResult.Get(), rcResultTitle);
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
HOOK(int, __fastcall, HudResult_CHudResultAddCallback, 0x10B8ED0, Sonic::CGameObject* This, void* Edx, int a2, int a3, int a4)
{
	int result = originalHudResult_CHudResultAddCallback(This, Edx, a2, a3, a4);
	HudResult_CHudResultRemoveCallback(This, nullptr, nullptr);

	Sonic::CCsdDatabaseWrapper wrapper(This->m_pMember->m_pGameDocument->m_pMember->m_spDatabase.get());

	auto spCsdProject = wrapper.GetCsdProject("uiRANGE_result");
	rcProjectResult = spCsdProject->m_rcProject;
	rcResultTitle = rcProjectResult->CreateScene("Scene_scanlines_range");
	CSDCommon::PlayAnimation(*rcResultTitle, "Usual_Anim", Chao::CSD::eMotionRepeatType_Loop, 1, 0);
	if (rcProjectResult && !spResult)
	{
		spResult = boost::make_shared<Sonic::CGameObjectCSD>(rcProjectResult, 0.5f, "HUD", false);
		Sonic::CGameDocument::GetInstance()->AddGameObject(spResult, "main", This);

	}
	return result;
}
HOOK(void, __fastcall, HudResult_CHudResultAdvance, 0x10B96D0, Sonic::CGameObject* This, void* Edx, const hh::fnd::SUpdateInfo& in_rUpdateInfo)
{
	
		originalHudResult_CHudResultAdvance(This, Edx, in_rUpdateInfo);
		
}
__declspec(naked) void ResultTimer_MidAsmHook()
{
	static void* returnAddress = (void*)0x00CFD566;
	static double timer = 0.00;

	__asm
	{
		push eax
		lea eax, timer
		movsd xmm1, [eax]
		pop eax
		jmp[returnAddress]
	}
}
void HudResult::Install()
{
	// Data collection
	INSTALL_HOOK(HudResult_CStateGoalFadeBeforeBegin);

	//INSTALL_HOOK(HudResult_CStateGoalFadeInBegin);

	// Result HUD handling
	INSTALL_HOOK(HudResult_CHudResultAddCallback);
	WRITE_MEMORY(0x16A1C38, void*, HudResult_CHudResultRemoveCallback);
	INSTALL_HOOK(HudResult_CHudResultAdvance);
	// Do not modify the results timer if Customizable Results Music is enabled.
	if (GetModuleHandle(TEXT("CustomizableResultsMusic.dll")) == nullptr)
		WRITE_JUMP(0xCFD55E, &ResultTimer_MidAsmHook);
}