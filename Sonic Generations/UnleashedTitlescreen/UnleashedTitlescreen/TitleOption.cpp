Chao::CSD::RCPtr<Chao::CSD::CProject> rcOptions;
Chao::CSD::RCPtr<Chao::CSD::CScene> headerBG, headerIMG, footerBG,footerIMG, worldmaptext, info1;
boost::shared_ptr<Sonic::CGameObjectCSD> spOptions;
Sonic::CGameObject* ogObj;
bool leaving = false;
void __fastcall CHudSonicStageRemoveCallback(Sonic::CGameObject* This, void*, Sonic::CGameDocument* pGameDocument)
{
	TitleOption::KillScreen();
	Chao::CSD::CProject::DestroyScene(rcOptions.Get(), headerBG);
	Chao::CSD::CProject::DestroyScene(rcOptions.Get(), headerIMG);
	Chao::CSD::CProject::DestroyScene(rcOptions.Get(), footerBG);
	Chao::CSD::CProject::DestroyScene(rcOptions.Get(), footerIMG);
	rcOptions = nullptr;
}
void TitleOption::CreateScreen(Sonic::CGameObject* pParentGameObject)
{
	if (rcOptions && !spOptions)
		pParentGameObject->m_pMember->m_pGameDocument->AddGameObject(spOptions = boost::make_shared<Sonic::CGameObjectCSD>(rcOptions, 0.5f, "HUD", false), "main", pParentGameObject);
}
void TitleOption::KillScreen()
{	
	if (spOptions)
	{
		spOptions->SendMessage(spOptions->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
		spOptions = nullptr;
	}
}
void TitleOption::ToggleScreen(const bool visible, Sonic::CGameObject* pParentGameObject)
{
	if (visible)
		CreateScreen(pParentGameObject);
	else
		KillScreen();
}
HOOK(void, __fastcall, TitleOption_CMain_CStateIntro, 0xD215A0, int a1, int a2, int a3) {

	Title::SetScrollDirection(false);
	Title::SetSubmenu(true);
	originalTitleOption_CMain_CStateIntro(a1, a2, a3);
	Title::SetHideEverything(true);
}
HOOK(void, __fastcall, TitleOption_CMain_CStateOutro, 0xD22A70, boost::shared_ptr<SaveLoadTestStruct>& spSave, void* a2, void* Edx)
{	
	if (!TitleWorldMap::Active)
	{
		leaving = true;
		Title::SetScrollDirection(true);
		Title::SetHideEverything(false);
		Title::SetSubmenu(false);
		if (headerBG)
			CSDCommon::PlayAnimation(*headerBG, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 60, 0, false, true);
		if (headerIMG)
			CSDCommon::PlayAnimation(*headerIMG, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 100, 0, false, true);
		if (footerBG)
			CSDCommon::PlayAnimation(*footerBG, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 60, 0, false, true);
		if (footerIMG)
			CSDCommon::PlayAnimation(*footerIMG, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 100, 0, false, true);
	}
	
	originalTitleOption_CMain_CStateOutro(spSave, a2,Edx);
	 
}
HOOK(void*, __fastcall, TitleOption_UpdateApplication, 0xE7BED0, Sonic::CGameObject* This, void* Edx, float elapsedTime, uint8_t a3)
{
	if (headerBG)
	{
		if (headerBG->m_MotionDisableFlag && leaving)
			Title::SetSubmenu(false);
	}
	return originalTitleOption_UpdateApplication(This, Edx, elapsedTime, a3);
}
HOOK(int, __fastcall, TitleOption_CMain, 0xD22850, Sonic::CGameObject* This, void* Edx, int a2, int a3, int* a4)
{
	ogObj = This;
	CHudSonicStageRemoveCallback(This, nullptr, nullptr);
	Sonic::CCsdDatabaseWrapper wrapper(This->m_pMember->m_pGameDocument->m_pMember->m_spDatabase.get());

	auto spCsdProject = wrapper.GetCsdProject("ui_worldmap");
	rcOptions = spCsdProject->m_rcProject;
	headerBG = rcOptions->CreateScene("worldmap_header_bg");
	headerIMG = rcOptions->CreateScene("worldmap_header_img");
	footerBG = rcOptions->CreateScene("worldmap_footer_bg");
	footerIMG = rcOptions->CreateScene("worldmap_footer_img");
	CSDCommon::PlayAnimation(*headerBG, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1,60);
	CSDCommon::PlayAnimation(*headerIMG, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1,0);
	CSDCommon::PlayAnimation(*footerBG, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1,60);
	CSDCommon::PlayAnimation(*footerIMG, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1,0);

	TitleOption::CreateScreen(This);

	return originalTitleOption_CMain(This, Edx, a2, a3, a4);
}

void TitleOption::Install()
{
	WRITE_MEMORY(0x16E14D8, void*, CHudSonicStageRemoveCallback);
	WRITE_JUMP(0x00573136, 0x00573142); //Remove options music
	INSTALL_HOOK(TitleOption_CMain);
	INSTALL_HOOK(TitleOption_CMain_CStateIntro);
	INSTALL_HOOK(TitleOption_UpdateApplication);
	INSTALL_HOOK(TitleOption_CMain_CStateOutro);
}