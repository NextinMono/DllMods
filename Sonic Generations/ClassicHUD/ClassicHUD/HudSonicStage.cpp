boost::shared_ptr<Sonic::CGameObjectCSD> goGameplay;
//Projects
Chao::CSD::RCPtr<Chao::CSD::CProject> pGameplay;
Chao::CSD::RCPtr<Chao::CSD::CScene> lifeCount;

void HudSonicStage::CreateScreen(Sonic::CGameObject* pParentGameObject)
{
	if (pGameplay && !goGameplay)
		pParentGameObject->m_pMember->m_pGameDocument->AddGameObject(goGameplay = boost::make_shared<Sonic::CGameObjectCSD>(pGameplay, 0.5f, "HUD", false), "main", pParentGameObject);
}
void HudSonicStage::KillScreen()
{
	if (goGameplay)
	{
		goGameplay->SendMessage(goGameplay->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
		goGameplay = nullptr;
	}
}
void HudSonicStage::ToggleScreen(const bool visible, Sonic::CGameObject* pParentGameObject)
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
//REMEMBER TO CALL NULLPTR HERE!!!!
void __fastcall CHudSonicStageRemoveCallback(Sonic::CGameObject* This, void*, Sonic::CGameDocument* pGameDocument)
{

	Chao::CSD::CProject::DestroyScene(pGameplay.Get(), lifeCount);
	HudSonicStage::KillScreen();

	pGameplay = nullptr;
}

HOOK(void, __fastcall, CHudSonicStageDelayProcessImp, 0x109A8D0, Sonic::CGameObject* This)
{
	originalCHudSonicStageDelayProcessImp(This);
	CHudSonicStageRemoveCallback(This, nullptr, nullptr);

	Sonic::CCsdDatabaseWrapper wrapper(This->m_pMember->m_pGameDocument->m_pMember->m_spDatabase.get());



	auto spCsdProject = wrapper.GetCsdProject("ui_gameplay");
	pGameplay = spCsdProject->m_rcProject;

	lifeCount = pGameplay->CreateScene("info_life");

	size_t& flags = ((size_t*)This)[151];
	bool isModern = (flags & 0x200);
	//Set modern icon
	lifeCount->GetNode("icon_generic")->SetPatternIndex(isModern ? 1 : 0);
	
		lifeCount->GetNode("Cast_0237")->SetHideFlag(isModern);
		lifeCount->GetNode("Cast_0235")->SetHideFlag(!isModern);

	
	
	//~(0x1 | /*0x2/* | 0x4 | 0x200 | */ | 0x800); 
	flags &= ~(0x1); // Mask to prevent crash when game tries accessing the elements we disabled later on, disables Lives & Rings count


	HudSonicStage::CreateScreen(This);
}

HOOK(void, __fastcall, CHudSonicStageUpdateParallel, 0x1098A50, Sonic::CGameObject* This, void* Edx, const hh::fnd::SUpdateInfo& in_rUpdateInfo)
{
	HudSonicStage::ToggleScreen(*(bool*)0x1A430D8, This); // ms_IsRenderGameMainHud
	originalCHudSonicStageUpdateParallel(This, Edx, in_rUpdateInfo);
	if (!goGameplay)
		return;
	char text[256];
	if (lifeCount)
	{
		const size_t liveCountAddr = Common::GetMultiLevelAddress(0x1E66B34, { 0x4, 0x1B4, 0x7C, 0x9FDC });
		if (liveCountAddr)
		{
			sprintf(text, "%d", *(size_t*)liveCountAddr);
			lifeCount->GetNode("num_sonic")->SetText(text);
		}
	}
}
HOOK(void, __fastcall, CHudSonicStageUpdate, 0x1098A50, void* thisDeclaration, void* edx, float* pUpdateInfo)
{
	// Force disable extended boost.
	*(uint32_t*)((uint32_t)*CONTEXT->ms_pInstance + 0x680) = 1;

	// Always clamp boost to 100.
	CONTEXT->m_ChaosEnergy = min(CONTEXT->m_ChaosEnergy, 100.0f);

	originalCHudSonicStageUpdate(thisDeclaration, edx, pUpdateInfo);
}
void HudSonicStage::Install()
{
	INSTALL_HOOK(CHudSonicStageDelayProcessImp);
	INSTALL_HOOK(CHudSonicStageUpdateParallel);
	INSTALL_HOOK(CHudSonicStageUpdate);
	WRITE_MEMORY(0x109B1A4, uint8_t, 0xE9, 0xDC, 0x02, 0x00, 0x00); // Disable lives (patched differently to not clash with Disable Lives patch)

	WRITE_MEMORY(0x16A467C, void*, CHudSonicStageRemoveCallback);
	WRITE_MEMORY(0x168D33C, const char, "%d");
	WRITE_MEMORY(0x0168560C, const char, "%d");
	WRITE_MEMORY(0x168E8E0, const char, "%d"); 
	WRITE_MEMORY(0x0168E8C4, const char, "%d");
	WRITE_MEMORY(0x0168E8CC, const char, "%d:%02d");
}