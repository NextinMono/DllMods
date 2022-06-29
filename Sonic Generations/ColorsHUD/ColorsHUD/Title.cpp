#include <thread>
Chao::CSD::RCPtr<Chao::CSD::CProject> rcTitleScreen;
Chao::CSD::RCPtr<Chao::CSD::CScene> rcTitleMain;
boost::shared_ptr<Sonic::CGameObjectCSD> spTitleScreen;
int currentTitleIndex = 0;

void Title::CreateScreen(Sonic::CGameObject* pParentGameObject)
{
	if (rcTitleScreen && !spTitleScreen)
		pParentGameObject->m_pMember->m_pGameDocument->AddGameObject(spTitleScreen = boost::make_shared<Sonic::CGameObjectCSD>(rcTitleScreen, 0.5f, "HUD", false), "main", pParentGameObject);
}

void Title::KillScreen()
{
	if (spTitleScreen)
	{
		spTitleScreen->SendMessage(spTitleScreen->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
		spTitleScreen = nullptr;
	}
}

void Title::ToggleScreen(const bool visible, Sonic::CGameObject* pParentGameObject)
{
	if (visible)
		CreateScreen(pParentGameObject);
	else
		KillScreen();
}

void ColorsTitleSelect()
{
	if (currentTitleIndex < 0) 
		currentTitleIndex = 3;
	else if (currentTitleIndex > 3)
		currentTitleIndex = 0;	

	std::string name = "position_" + std::to_string(currentTitleIndex);
	CSDCommon::PlayAnimation(*rcTitleMain, name.c_str(), Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);

#if _DEBUG
	printf("\nCURRENT INDEX: %d", currentTitleIndex);
#endif

}

HOOK(int, __fastcall, CTitleMain, 0x0056FBE0, Sonic::CGameObject* This, void* Edx, int a2, int a3, void** a4)
{
	//Spawn
	rcTitleScreen = nullptr;
	originalCTitleMain(This, Edx, a2, a3, a4);
	rcTitleScreen = nullptr;

	Sonic::CCsdDatabaseWrapper wrapper(This->m_pMember->m_pGameDocument->m_pMember->m_spDatabase.get());

	auto spCsdProject = wrapper.GetCsdProject("ui_title_colors");
	rcTitleScreen = spCsdProject->m_rcProject;
	rcTitleMain = rcTitleScreen->CreateScene("title");
	CSDCommon::FreezeMotion(*rcTitleMain);
	CSDCommon::PlayAnimation(*rcTitleMain, "main_Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	Title::CreateScreen(This);
	return 0;
}

HOOK(int, __fastcall, CTitleMainSelect, 0x11D1210, int a1)
{
	//When Press Start is done
	CSDCommon::FreezeMotion(*rcTitleMain);
	CSDCommon::PlayAnimation(*rcTitleMain, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	return originalCTitleMainSelect(a1);
}

HOOK(int, __fastcall, CTitleMainWait, 0x11D1410, int a1)
{
	/*if(rcTitleMain->m_MotionDisableFlag == 1)
	CSDCommon::PlayAnimation(*rcTitleMain, "main_Active_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, -1, rcTitleMain->m_MotionEndFrame);*/

	return originalCTitleMainWait(a1);
}

HOOK(int, __fastcall, CTitleMainFinish, 0x5727F0, Sonic::CGameObject* This)
{
	auto inputState = Sonic::CInputState::GetInstance();
	auto inputPtr = &inputState->m_PadStates[inputState->m_CurrentPadStateIndex];
	if (rcTitleMain->m_MotionDisableFlag == 0)
		return 0;
	if (inputPtr->LeftStickVertical >= 0.5f)
		currentTitleIndex--;
	else if(inputPtr->LeftStickVertical <= -0.5f)
		currentTitleIndex++;

	ColorsTitleSelect();
	return 0;
}
void Title::Install()
{
	//if (!StringHelper::IsEmpty(Configuration::UIType.c_str()))
	//{
	//	// Loop the title screen animations indefinitely (code by Hyper).
	//	WRITE_MEMORY(0x1A57A3C, uint8_t, 0x00);
	//	WRITE_MEMORY(0x1A57BBC, uint8_t, 0x00);

	//	// Disable the title outro animation (code by Skyth).
	//	WRITE_MEMORY(0x57346F, uint32_t, 0x16A36CC);
	//}
	INSTALL_HOOK(CTitleMain);
	INSTALL_HOOK(CTitleMainFinish);
	INSTALL_HOOK(CTitleMainWait);
	INSTALL_HOOK(CTitleMainSelect);
}