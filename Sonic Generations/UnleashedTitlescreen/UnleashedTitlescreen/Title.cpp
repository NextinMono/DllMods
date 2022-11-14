Chao::CSD::RCPtr<Chao::CSD::CProject> rcTitleScreen;
Chao::CSD::RCPtr<Chao::CSD::CScene> rcTitleLogo_1, rcTitlebg, rcTitleMenu, rcTitleMenuScroll, rcTitleMenuTXT;
boost::shared_ptr<Sonic::CGameObjectCSD> spTitleScreen;
uint32_t currentTitleIndex = 0;
int timePassed = 1;
bool entered = false;
bool submenu = false;
bool startAnimComplete, startButtonAnimComplete, startBgAnimComplete = false;
bool moved = false;
bool hasSavefile = false;
bool reversedAnim = false;
bool inTitle = true;
bool holdingStick = false;
bool fadeOut = false;
int holdTimer = 0;
int executionState = 0;
bool pressedA;
bool enteredMenu = false;
static int movementInt; //has to be -1 or 1
FUNCTION_PTR(void, __thiscall, TitleUI_TinyChangeState, 0x773250, hh::fnd::CStateMachineBase::CStateBase* This, SharedPtrTypeless& spState, const Hedgehog::Base::CSharedString name);
HOOK(void, __fastcall, TitleUI_TitleCMainCState_SelectMenuAdvance, 0x5728F0, hh::fnd::CStateMachineBase::CStateBase* This) 
{
	uint32_t owner = (uint32_t)(This->GetContextBase());
	uint32_t* outState = (uint32_t*)(owner + 0x1BC);
	static SharedPtrTypeless spOutState;
	//0: new game, 1/4: continue, 
	*outState = currentTitleIndex;
	if(pressedA)
	TitleUI_TinyChangeState(This, spOutState, fadeOut ? "Init" : "ExecSubMenu");
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
void __declspec(naked) TitleUI_DisableOnlineModeButton()
{
	static uint32_t pAddr = 0x005708D7;
	static uint32_t pAddr2 = 0x00570901;
	__asm
	{
		/*cmp     ebp, -1*/
		jmp[pAddr2]
	}
}
void __fastcall CTitleRemoveCallback(Sonic::CGameObject* This, void*, Sonic::CGameDocument* pGameDocument)
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
void Title::SetHideEverything(bool visible) {
	rcTitleLogo_1->SetHideFlag(visible);
	rcTitlebg->SetHideFlag(visible);
	rcTitleMenu->SetHideFlag(visible);
	rcTitleMenuScroll->SetHideFlag(visible);
	rcTitleMenuTXT->SetHideFlag(visible);
}
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
	printf("\nCURRENT currentTitleIndex: %d", currentTitleIndex);
#endif

	/*CSDCommon::PlayAnimation(*rcTitleMenuTXT, "Intro_Anim_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);*/
}
HOOK(void, __fastcall, Title_CMain_CState_SelectMenuBegin, 0x572750, hh::fnd::CStateMachineBase::CStateBase* This)
{
	/*originalTitle_CMain_CState_SelectMenuBegin(This);*/
	submenu = false;
	uint32_t owner = (uint32_t)(This->GetContextBase());
	hasSavefile = *(bool*)(owner + 0x1AC);
	currentTitleIndex = hasSavefile ? 1 : 0;
	UnleashedTitleText();

}

HOOK(int, __fastcall, Title_CMain, 0x0056FBE0, Sonic::CGameObject* This, void* Edx, int a2, int a3, void** a4)
{
	CTitleRemoveCallback(This, nullptr, nullptr);

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
	CSDCommon::FreezeMotion(*rcTitleMenuTXT);
	CSDCommon::FreezeMotion(*rcTitleMenuScroll);
	CSDCommon::FreezeMotion(*rcTitleMenu);
	CSDCommon::FreezeMotion(*rcTitlebg);
	CSDCommon::PlayAnimation(*rcTitleMenu, "Intro_Anim_1", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*rcTitlebg, "Intro_Anim_1", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*rcTitleMenuScroll, "Scroll_Anim_2_1", Chao::CSD::eMotionRepeatType_PlayOnce, 0, 0);
	CSDCommon::FreezeMotion(*rcTitleMenuScroll, 0);
	CSDCommon::PlayAnimation(*rcTitleLogo_1, "Intro_Anim_1", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	rcTitleMenuScroll->SetPosition(0, 650000);
	rcTitleMenuTXT->SetPosition(0, 650000);
	Title::CreateScreen(This);

	return originalTitle_CMain(This, Edx, a2, a3, a4);
}

HOOK(DWORD, *__cdecl, Title_CMain_CState_SelectMenu, 0x11D1210, hh::fnd::CStateMachineBase::CStateBase* This)
{
	if (entered)
		return originalTitle_CMain_CState_SelectMenu(This);
	CSDCommon::FreezeMotion(*rcTitleMenu);
	CSDCommon::PlayAnimation(*rcTitlebg, "Intro_Anim_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*rcTitleMenu, "Intro_Anim_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*rcTitleMenuTXT, "Intro_Anim_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	entered = true;
	rcTitleMenuTXT->SetPosition(0, 0);
	UnleashedTitleText();
	rcTitleMenuScroll->SetPosition(0, 0);

	return originalTitle_CMain_CState_SelectMenu(This);
}

HOOK(int, __fastcall, Title_CMain_CState_WaitStart, 0x11D1410, int a1)
{
	if (entered) {
		CSDCommon::PlayAnimation(*rcTitleMenu, "Intro_Anim_1", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, false, true);
		CSDCommon::PlayAnimation(*rcTitlebg, "Intro_Anim_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 30, 100, false, true);
		currentTitleIndex = 0;
	}
	rcTitleMenuTXT->SetPosition(0, 65000);
	rcTitleMenuScroll->SetPosition(0, 65000);
	reversedAnim = true;
	inTitle = true;
	entered = false;
	return originalTitle_CMain_CState_WaitStart(a1);
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
	auto inputState = Sonic::CInputState::GetInstance();
	auto inputPtr = &inputState->m_PadStates[inputState->m_CurrentPadStateIndex];
	pressedA = inputPtr->IsDown(Sonic::eKeyState_A);
	if (submenu && inputPtr->IsDown(Sonic::eKeyState_B))
		submenu = false;
	if (rcTitleLogo_1)
	{
		if (rcTitleLogo_1->m_MotionDisableFlag == 1 && !startAnimComplete)
		{
			startAnimComplete = true;
			CSDCommon::PlayAnimation(*rcTitleLogo_1, "Usual_Anim_1", Chao::CSD::eMotionRepeatType_Loop, 1, 0);

		}
	}

	if (rcTitleMenu)
	{
		/*if (rcTitleMenu->m_MotionDisableFlag == 1 && !startButtonAnimComplete)
		{
			startButtonAnimComplete = true;
			CSDCommon::PlayAnimation(*rcTitleMenu, "Usual_Anim_1", Chao::CSD::eMotionRepeatType_Loop, 1, 0);
		}*/
		/*if (reversedAnim) {
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
		}*/
	}
	/*if (rcTitlebg && entered)
	{
		if (rcTitlebg->m_MotionDisableFlag == 1 && !startBgAnimComplete)
		{
			startBgAnimComplete = true;
			CSDCommon::PlayAnimation(*rcTitlebg, "Usual_Anim_2", Chao::CSD::eMotionRepeatType_Loop, 1, 0);
		}
	}*/
	if (rcTitleMenu && !submenu)
	{
		if (IsRightDown() && !moved)
		{
			if (currentTitleIndex > 3)
				currentTitleIndex = 0;

			currentTitleIndex += 1;
			CSDCommon::PlayAnimation(*rcTitleMenuScroll, "Scroll_Anim_2_1", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
			CSDCommon::PlayAnimation(*rcTitleMenu, "Scroll_Anim_2_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 100, false, true);
			UnleashedTitleText();
			moved = true;


		}
		if (IsLeftDown() && !moved)
		{
			if (currentTitleIndex < 0)
				currentTitleIndex = 3;

			currentTitleIndex -= 1;
			CSDCommon::PlayAnimation(*rcTitleMenuScroll, "Scroll_Anim_2_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
			CSDCommon::PlayAnimation(*rcTitleMenu, "Scroll_Anim_2_1", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 100, false, true);
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
			if (holdTimer > 20)
			{
				if (currentTitleIndex > 0 && currentTitleIndex < 3)
					moved = false;
				holdTimer = 0;
			}
		}
	}
	return originalTitle_UpdateApplication(This, Edx, elapsedTime, a3);
}

HOOK(int, __fastcall, sub_A53400, 0xA53400)
{
	return -1;
}
HOOK(void, __fastcall, sub_647CB0, 0x647CB0, const char* This, int a2, int a3)
{
	return;
}
HOOK(void, __fastcall, Title_CMain_CState_Init, 0x571430, hh::fnd::CStateMachineBase::CStateBase* This)
{
	printf("Title_CMain_CState_Init");
	originalTitle_CMain_CState_Init(This);
}
HOOK(void, __fastcall, ExecSubMenu, 0x5732A0, void* This)
{
	submenu = true;
	originalExecSubMenu(This);
}
void __declspec(naked) TitleUI_MoveUpPress()
{
	static uint32_t pAddr = 0x010BA69C;
	__asm
	{
		inc     dword ptr[edi + 14h]
		mov     byte ptr[edi + 5], 0
		jmp[pAddr]
	}
}
void __declspec(naked) TitleUI_SetCustomExecFunction()
{
	//https://godbolt.org/
	static uint32_t pAddr = 0x00572E27;
	static uint32_t adNewGame = 0x00572D33;
	static uint32_t adContinue = 0x00572E2F;
	static uint32_t adOptions = 0x00573008;
	static uint32_t adQuit = 0x00573153;
	__asm // this is an else if chain. i wanted to do a switch statement but it didnt work.
	{
		cmp     currentTitleIndex, 0
		jne	Continue
		jmp[adNewGame]
		jmp	FunctionFinish

		Continue:
			cmp     currentTitleIndex, 1
			jne	Options
			jmp[adContinue]
			jmp	FunctionFinish

		Options:
			cmp     currentTitleIndex, 2
			jne Quit
			jmp[adOptions]
			jmp	FunctionFinish

		Quit:
			cmp     currentTitleIndex, 3
			jne	FunctionFinish
			jmp[adQuit]

		FunctionFinish:
			jmp[pAddr]			
	}
}
void __declspec(naked) TitleUI_SetCustomExecFunctionAdvance()
{
	//https://godbolt.org/
	static uint32_t pAddr = 0x0057372C;
	static uint32_t adNewGame = 0x0057339E;
	static uint32_t adContinue = 0x0057342B;
	static uint32_t adOptions = 0x00573557;
	static uint32_t adQuit = 0x0057364F;
	__asm // NEW: 0| CONTINUE:1 | OPTIONS: 3 | QUIT:  4
	{
		cmp     currentTitleIndex, 0
		jne	Continue
		mov submenu, 0
		jmp[adNewGame]
		jmp	FunctionFinish

		Continue :
		cmp     currentTitleIndex, 1
			jne	Options
			jmp[adContinue]
			jmp	FunctionFinish

			Options :
		cmp     currentTitleIndex, 2
			jne Quit
			jmp[adOptions]
			jmp	FunctionFinish

			Quit :
		cmp     currentTitleIndex, 3
			jne	FunctionFinish
			jmp[adQuit]
			mov submenu, 1

			FunctionFinish :
			jmp[pAddr]
	}
}
HOOK(char, __fastcall, Finish, 0x5727F0, void* This)
{
	return 0;
}
HOOK(void, __fastcall, sub_5732A0, 0x5732A0, DWORD* This) 
{
	FUNCTION_PTR(int, __fastcall, sub_772C20, 0x772C20, DWORD * This);
	auto v2 = sub_772C20(This);
	auto result = (*(int(__thiscall**)(int))(*(DWORD*)v2 + 56))(v2);
	return originalsub_5732A0(This);
}
HOOK(int, __fastcall, ExecSubMenuu, 0x572D00, DWORD* This)
{
	FUNCTION_PTR(int, __fastcall, sub_772C20, 0x772C20, DWORD * This);
	auto v2 = sub_772C20(This);
	int returned = 0;
	auto result = (*(int(__thiscall**)(int))(*(DWORD*)v2 + 56))(v2);
	if (currentTitleIndex == 3 && !submenu)
	{
		submenu = true;
		returned = originalExecSubMenuu(This);
	}
	else if (currentTitleIndex != 3)
		submenu = false;

	if(!submenu)
		return originalExecSubMenuu(This);
	else 
		return returned; //will 9/10 times return 0
	
}
void Title::SetSubmenu(bool enabled)
{
	submenu = enabled;
}
void Title::Install()
{
	//Set up title screen so that it resembles Unleashed function-wise
	WRITE_MEMORY(0x015686E4, float, 93); //Set title AFK wait amount - it varies depending on framerate
	WRITE_JUMP(0x010BA68D, TitleUI_MoveUp);
	WRITE_JUMP(0x010BA69E, TitleUI_MoveDown);
	WRITE_JUMP(0x010BA695, TitleUI_MoveUpPress);
	WRITE_MEMORY(0x1A43100, byte, 0);
	WRITE_MEMORY(0x1A43101, byte, 0);
	WRITE_MEMORY(0x1A430D4, byte, 0);
	WRITE_JUMP(0x005708DB, TitleUI_DisableOnlineModeButton);
	WRITE_JUMP(0x00572D23, TitleUI_SetCustomExecFunction);
	WRITE_JUMP(0x005732C3, TitleUI_SetCustomExecFunctionAdvance);
	WRITE_MEMORY(0x005709F0, uint8_t, 3);

	//UI
	WRITE_MEMORY(0x016E11F4, void*, CTitleRemoveCallback);
	INSTALL_HOOK(Title_UpdateApplication);
	INSTALL_HOOK(ExecSubMenuu);
	INSTALL_HOOK(sub_5732A0);
	INSTALL_HOOK(sub_A53400);
	WRITE_JUMP(0x572D3A, (void*)0x57326B); // Don't create delete save dialog
	WRITE_JUMP(0x5732D3, (void*)0x573337); // Always delete save on new game
	//State Hooks
	INSTALL_HOOK(Title_CMain);
	INSTALL_HOOK(Title_CMain_CState_SelectMenuBegin);
	INSTALL_HOOK(Title_CMain_CState_Init);
	INSTALL_HOOK(Title_CMain_CState_WaitStart);
	INSTALL_HOOK(Title_CMain_CState_SelectMenu);
	//INSTALL_HOOK(TitleUI_TitleCMainCState_SelectMenuAdvance);
	//INSTALL_HOOK(Finish);

	//Other
	INSTALL_HOOK(sub_647CB0);
}