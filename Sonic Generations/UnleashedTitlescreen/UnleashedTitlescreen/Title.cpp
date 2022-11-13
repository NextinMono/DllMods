Chao::CSD::RCPtr<Chao::CSD::CProject> rcTitleScreen;
Chao::CSD::RCPtr<Chao::CSD::CProject> rcWorldMap;
Chao::CSD::RCPtr<Chao::CSD::CScene> rcTitleLogo_1, rcTitlebg, rcTitleMenu, rcTitleMenuScroll, rcTitleMenuTXT;
Chao::CSD::RCPtr<Chao::CSD::CScene> header, footer, worldmaptext, info1;
boost::shared_ptr<Sonic::CGameObjectCSD> spTitleScreen, spWorldMap;
int currentTitleIndex = 0;
int timePassed = 1;
bool entered = false;
bool startAnimComplete, startButtonAnimComplete, startBgAnimComplete = false;
bool moved = false;
bool hasSavefile = false;
bool reversedAnim = false;
bool inTitle = true;
bool holdingStick = false;
int holdTimer = 0;
bool enteredMenu = false;
static int movementInt; //has to be -1 or 1

inline FUNCTION_PTR(int, __fastcall, CTitleMainFinishH, 0x5727F0, void* Edx, Sonic::CGameObject* This);

void Title::CreateScreen(Sonic::CGameObject* pParentGameObject)
{
	if (rcTitleScreen && !spTitleScreen)
		pParentGameObject->m_pMember->m_pGameDocument->AddGameObject(spTitleScreen = boost::make_shared<Sonic::CGameObjectCSD>(rcTitleScreen, 0.5f, "HUD", false), "main", pParentGameObject);
	if (rcWorldMap && !spWorldMap)
		pParentGameObject->m_pMember->m_pGameDocument->AddGameObject(spWorldMap = boost::make_shared<Sonic::CGameObjectCSD>(rcWorldMap, 0.5f, "HUD", false), "main", pParentGameObject);
}

void Title::KillScreen()
{
	if (spTitleScreen)
	{
		spTitleScreen->SendMessage(spTitleScreen->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
		spTitleScreen = nullptr;
	}
	if (spWorldMap)
	{
		spWorldMap->SendMessage(spWorldMap->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
		spWorldMap = nullptr;
	}
}

void Title::ToggleScreen(const bool visible, Sonic::CGameObject* pParentGameObject)
{
	if (visible)
		CreateScreen(pParentGameObject);
	else
		KillScreen();
}

void Title::FreezeMotion(Chao::CSD::CScene* pScene)
{
	pScene->SetMotionFrame(pScene->m_MotionEndFrame);
	pScene->m_MotionSpeed = 0.0f;
	pScene->m_MotionRepeatType = Chao::CSD::eMotionRepeatType_PlayOnce;
}

void Title::PlayAnimation(Chao::CSD::CScene* pScene, const char* name, Chao::CSD::EMotionRepeatType repeatType, float motionSpeed, float startFrame)
{
	pScene->SetMotion(name);
	pScene->m_MotionRepeatType = repeatType;
	pScene->m_MotionDisableFlag = 0;
	pScene->m_MotionSpeed = motionSpeed;
	pScene->SetMotionFrame(startFrame);
	pScene->Update(0);
}
void StuffTest(int number)
{
	static uint32_t pAddr = 0x005727FE;
	__asm
	{
		mov edi, number
		jmp pAddr
	}
}

void Title::IntroAnim(Chao::CSD::RCPtr<Chao::CSD::CScene> scene)
{
	scene->SetMotion("Intro_Anim");
	scene->SetMotionFrame(0.0f);
	scene->m_MotionSpeed = 1;
	scene->m_MotionRepeatType = Chao::CSD::eMotionRepeatType_PlayOnce;
	scene->Update(0.0f);
}

void UnleashedTitleText()
{
	rcTitleMenuTXT->SetPosition(0, 0);

	if (currentTitleIndex < 0)
		currentTitleIndex = 3;
	else if (currentTitleIndex > 3)
		currentTitleIndex = 0;

	rcTitleMenuTXT->GetNode("txt_0")->SetHideFlag(!(currentTitleIndex == 0));
	rcTitleMenuTXT->GetNode("txt_1")->SetHideFlag(!(currentTitleIndex == 1));
	rcTitleMenuTXT->GetNode("txt_2")->SetHideFlag(!(currentTitleIndex == 2));
	rcTitleMenuTXT->GetNode("txt_3")->SetHideFlag(!(currentTitleIndex == 3));
	rcTitleMenuTXT->GetNode("txt_4")->SetHideFlag(!(currentTitleIndex == 4));

#if _DEBUG
	printf("\nCURRENT INDEX: %d", currentTitleIndex);
#endif

	/*Title::PlayAnimation(*rcTitleMenuTXT, "Intro_Anim_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);*/
}
HOOK(void, __fastcall, CMainCState_SelectMenuBegin, 0x572750, hh::fnd::CStateMachineBase::CStateBase* This)
{
	originalCMainCState_SelectMenuBegin(This);
	uint32_t owner = (uint32_t)(This->GetContextBase());
	hasSavefile = *(bool*)(owner + 0x1AC);
	currentTitleIndex = hasSavefile ? 1 : 0;
	UnleashedTitleText();

}
HOOK(int, __fastcall, CTitleMain, 0x0056FBE0, Sonic::CGameObject* This, void* Edx, int a2, int a3, void** a4)
{
	rcTitleScreen = nullptr;
	rcTitleScreen = nullptr;

	Sonic::CCsdDatabaseWrapper wrapper(This->m_pMember->m_pGameDocument->m_pMember->m_spDatabase.get());

	auto spCsdProject = wrapper.GetCsdProject("ui_title_unleashed");
	rcTitleScreen = spCsdProject->m_rcProject;
	Configuration::Read();
	switch (Configuration::LogoType)
	{
	case 0:
	{
		rcTitleLogo_1 = rcTitleScreen->CreateScene("title_1");
		break;
	}
	case 1:
	{
		rcTitleLogo_1 = rcTitleScreen->CreateScene("title_2");
		break;
	}
	case 2:
	{
		rcTitleLogo_1 = rcTitleScreen->CreateScene("title_1");
		break;
	}
	}
	rcTitlebg = rcTitleScreen->CreateScene("bg");
	rcTitleMenu = rcTitleScreen->CreateScene("menu");
	rcTitleMenuScroll = rcTitleScreen->CreateScene("menu_scroll");
	rcTitleMenuTXT = rcTitleScreen->CreateScene("txt");
	rcTitleMenuTXT->GetNode("txt_4")->SetHideFlag(true);
	Title::FreezeMotion(*rcTitleMenuTXT);
	Title::FreezeMotion(*rcTitleMenuScroll);
	Title::FreezeMotion(*rcTitleMenu);
	Title::FreezeMotion(*rcTitlebg);
	Title::PlayAnimation(*rcTitleMenu, "Intro_Anim_1", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	Title::PlayAnimation(*rcTitlebg, "Intro_Anim_1", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	Title::PlayAnimation(*rcTitleMenuScroll, "Scroll_Anim_2_1", Chao::CSD::eMotionRepeatType_PlayOnce, 0, 0);
	Title::FreezeMotion(*rcTitleMenuScroll);
	Title::PlayAnimation(*rcTitleLogo_1, "Intro_Anim_1", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	rcTitleMenuScroll->SetPosition(0, 650000);
	rcTitleMenuTXT->SetPosition(0, 650000);
	Title::CreateScreen(This);

	return	originalCTitleMain(This, Edx, a2, a3, a4);
}

HOOK(DWORD, *__cdecl, CTitleMainSelect, 0x11D1210, hh::fnd::CStateMachineBase::CStateBase* This)
{
	if(entered)
		return originalCTitleMainSelect(This);
	Title::FreezeMotion(*rcTitleMenu);
	Title::PlayAnimation(*rcTitlebg, "Intro_Anim_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	Title::PlayAnimation(*rcTitleMenu, "Intro_Anim_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	Title::PlayAnimation(*rcTitleMenuTXT, "Intro_Anim_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	entered = true;
	rcTitleMenuTXT->SetPosition(0, 0);
	UnleashedTitleText();
	rcTitleMenuScroll->SetPosition(0, 0);

	return originalCTitleMainSelect(This);
}

HOOK(int, __fastcall, CTitleMainWait, 0x11D1410, int a1)
{


	if (entered) {
		CSDCommon::PlayAnimation(*rcTitleMenu, "Intro_Anim_1", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, false, true);
		CSDCommon::PlayAnimation(*rcTitlebg, "Intro_Anim_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, false, true);
	}
	rcTitleMenuTXT->SetPosition(0, 65000);
	rcTitleMenuScroll->SetPosition(0, 65000);
	reversedAnim = true;
	inTitle = true;
	entered = false;
	return originalCTitleMainWait(a1);
}
bool IsLeftDown() {
	auto inputState = Sonic::CInputState::GetInstance();
	auto inputPtr = &inputState->m_PadStates[inputState->m_CurrentPadStateIndex];
	return inputPtr->IsDown(Sonic::eKeyState_LeftStickLeft) || inputPtr->IsDown(Sonic::eKeyState_DpadLeft);
}
bool IsRightDown() {
	auto inputState = Sonic::CInputState::GetInstance();
	auto inputPtr = &inputState->m_PadStates[inputState->m_CurrentPadStateIndex];
	return inputPtr->IsDown(Sonic::eKeyState_LeftStickRight) || inputPtr->IsDown(Sonic::eKeyState_DpadRight);
}
float m_applicationDeltaTimeT = 0.0f;
HOOK(void*, __fastcall, Title_UpdateApplication, 0xE7BED0, Sonic::CGameObject* This, void* Edx, float elapsedTime, uint8_t a3)
{
	m_applicationDeltaTimeT = elapsedTime;
	timePassed += 1;
	/*	FUNCTION_PTR(int, __cdecl, FirstSetup, 0x11D10A0, int);*/
	if (rcTitleLogo_1)
	{
		if (rcTitleLogo_1->m_MotionDisableFlag == 1 && !startAnimComplete)
		{
			startAnimComplete = true;
			Title::PlayAnimation(*rcTitleLogo_1, "Usual_Anim_1", Chao::CSD::eMotionRepeatType_Loop, 1, 0);

		}
	}

	if (rcTitleMenu)
	{
		if (rcTitleMenu->m_MotionDisableFlag == 1 && !startButtonAnimComplete)
		{
			startButtonAnimComplete = true;
			Title::PlayAnimation(*rcTitleMenu, "Usual_Anim_1", Chao::CSD::eMotionRepeatType_Loop, 1, 0);
		}
		if (reversedAnim) {
			if (rcTitleMenu->m_MotionFrame <= 0)
			{
				CSDCommon::FreezeMotion(*rcTitleMenu);
			}
			if (rcTitlebg->m_MotionFrame <= 0)
			{
				CSDCommon::FreezeMotion(*rcTitlebg);
			}
			if (rcTitleMenuScroll->m_MotionFrame <= 0)
			{
				CSDCommon::FreezeMotion(*rcTitlebg);
			}
			reversedAnim = !(rcTitleMenu->m_MotionDisableFlag && rcTitlebg->m_MotionDisableFlag && rcTitleMenuScroll->m_MotionDisableFlag);
		}
	}
	if (rcTitlebg && entered)
	{
		if (rcTitlebg->m_MotionDisableFlag == 1 && !startBgAnimComplete)
		{
			startBgAnimComplete = true;
			Title::PlayAnimation(*rcTitlebg, "Usual_Anim_2", Chao::CSD::eMotionRepeatType_Loop, 1, 0);
		}
	}
	if (rcTitleMenu) {




		if (IsRightDown() && !moved)
		{
			if (currentTitleIndex > 3)
				currentTitleIndex = 0;

			currentTitleIndex += 1;
			Title::PlayAnimation(*rcTitleMenu, "Scroll_Anim_2_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
			UnleashedTitleText();
			moved = true;


		}
		if (IsLeftDown() && !moved)
		{
			if (currentTitleIndex < 0)
				currentTitleIndex = 3;

			currentTitleIndex -= 1;
			Title::PlayAnimation(*rcTitleMenu, "Scroll_Anim_2_1", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
			UnleashedTitleText();

			moved = true;
		}
		
		if ((!IsLeftDown() && !IsRightDown()) && moved)
		{
			holdTimer += 1;
			moved = false;
			holdTimer = 0;
		}

		if ((IsLeftDown() || IsRightDown()) && moved)
		{
			holdTimer = holdTimer + 1.0f;
			printf("\nTIMER %d", holdTimer);
			if (holdTimer > 20 )
			{
				if(currentTitleIndex > 0 && currentTitleIndex < 3)
				moved = false;
				holdTimer = 0;
			}
		}


	}






	return originalTitle_UpdateApplication(This, Edx, elapsedTime, a3);
}
void __declspec(naked) TitleUI_TitleScroll()
{
	static uint32_t pAddr = 0x00572837;
	__asm
	{
		mov edi, 1
		jmp[pAddr]
	}
}
void __declspec(naked) TitleUI_MoveUp()
{
	static uint32_t pAddr = 0x010BA693;
	static uint32_t movement = 0x80000;
	if (inTitle) //this is global to all of gens for some reason
		movement = 0x200000;
	else
		movement = 0x80000;
	__asm
	{
		test    esi, movement
		jmp[pAddr]
	}
}
void __declspec(naked) TitleUI_DisableOnlineModeButton()
{
	static uint32_t pAddr = 0x005708D7;
	static uint32_t pAddr2 = 0x00570905;
	__asm
	{
		cmp     ebp, -1
		jmp[pAddr2]
	}
}
void OnPressedUp() {
	printf("Pressed up");
}
void OnPressedDown() {
	printf("Pressed down");
}
void __declspec(naked) TitleUI_MoveDown()
{
	static uint32_t pAddr = 0x010BA6A4;
	static uint32_t movement = 0x40000;
	if (inTitle) //this is global to all of gens for some reason
		movement = 0x100000;
	else
		movement = 0x40000;
	__asm
	{
		test    ebx, movement
		jmp[pAddr]
	}
}

void __declspec(naked) TitleUI_MoveUpPress()
{
	static uint32_t pAddr = 0x010BA69C;
	__asm
	{
		call OnPressedUp
		inc     dword ptr[edi + 14h]
		mov     byte ptr[edi + 5], 0
		jmp[pAddr]
	}
}
void __declspec(naked) TitleUI_MoveDownPress()
{
	static uint32_t pAddr = 0x010BA6A4;
	static uint32_t loc = 0x010BA69E;
	__asm
	{		
		
		test    esi, 80000h
		jnz      downpress

		downpress:
			test    ebx, 40000h
			call OnPressedDown
		jmp[pAddr]
	}
}


void __fastcall CHudSonicStageRemoveCallback(Sonic::CGameObject* This, void*, Sonic::CGameDocument* pGameDocument)
{
	Title::KillScreen();
	Chao::CSD::CProject::DestroyScene(rcTitleScreen.Get(), rcTitlebg);
	Chao::CSD::CProject::DestroyScene(rcTitleScreen.Get(), rcTitleLogo_1);
	Chao::CSD::CProject::DestroyScene(rcTitleScreen.Get(), rcTitleMenu);
	Chao::CSD::CProject::DestroyScene(rcTitleScreen.Get(), rcTitleMenuScroll);
	Chao::CSD::CProject::DestroyScene(rcTitleScreen.Get(), rcTitleMenuTXT);
	rcTitleScreen = nullptr;
	entered = false;
	inTitle = false;
	currentTitleIndex = 0;
}

HOOK(void, __fastcall, sub_647CB0, 0x647CB0, const char* This, int a2, int a3)
{
	return;
}

HOOK(int, __fastcall, ExecSubMenu, 0x572D00, DWORD* This)
{
	FUNCTION_PTR(int, __fastcall, sub_772C20, 0x772C20, DWORD* This);
	auto v2 = sub_772C20(This);
	auto result = (*(int(__thiscall**)(int))(*(DWORD*)v2 + 56))(v2);

	///2: online mode
	///3: options
	///6: quit
	switch (result)
	{
	case 2: return 0;
	default:return originalExecSubMenu(This);
	}
}
void Title::Install()
{
	WRITE_JUMP(0x010BA68D, TitleUI_MoveUp);
	WRITE_JUMP(0x010BA69E, TitleUI_MoveDown);
	WRITE_JUMP(0x010BA695, TitleUI_MoveUpPress);
	WRITE_JUMP(0x005708DB, TitleUI_DisableOnlineModeButton);
	WRITE_MEMORY(0x016E11F4, void*, CHudSonicStageRemoveCallback);
	WRITE_MEMORY(0X005709F0, uint8_t, 3);
	WRITE_MEMORY(0x015686E4, float, 93); //Set title AFK wait amount
	INSTALL_HOOK(CMainCState_SelectMenuBegin);
	INSTALL_HOOK(Title_UpdateApplication);
	INSTALL_HOOK(sub_647CB0);
	INSTALL_HOOK(ExecSubMenu);
	INSTALL_HOOK(CTitleMain);
	INSTALL_HOOK(CTitleMainWait);
	INSTALL_HOOK(CTitleMainSelect);
}