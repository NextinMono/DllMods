Chao::CSD::RCPtr<Chao::CSD::CProject> rcTitleScreen;
Chao::CSD::RCPtr<Chao::CSD::CScene> rcTitleLogo_1, rcTitlebg, rcTitleMenu, rcTitleMenuScroll, rcTitleMenuTXT;
boost::shared_ptr<Sonic::CGameObjectCSD> spTitleScreen;

Title::TitleIndexState currentTitleIndex = Title::TitleIndexState::New_Game;
bool enteredStart, isInSubmenu, moved, hasSavefile, playingStartAnim, reversedAnim, isStartAnimComplete, startButtonAnimComplete, startBgAnimComplete, parsedSave = false;
bool inTitle, scrollHorizontally = true;
int holdTimer = 0;
boost::shared_ptr<Sonic::CCamera> spCamera;
Sonic::CCamera* cameraa2;
static uint32_t cameraInitRan = 0;
uint32_t* Title::OutState;
FUNCTION_PTR(void, __thiscall, TitleUI_TinyChangeState, 0x773250, void* This, SharedPtrTypeless& spState, const Hedgehog::Base::CSharedString name);


void Title::SetSubmenu(bool enabled)
{
	isInSubmenu = enabled;
}
void Title::SetScrollDirection(bool horizontal)
{
	scrollHorizontally = horizontal;
}
void ExitingTitle()
{
	Title::SetScrollDirection(false);
	Title::SetSubmenu(false);
	inTitle = false;
	isStartAnimComplete = false;
}


void __declspec(naked) TitleUI_SetCutsceneTimer()
{
	static uint32_t pAddr = 0x00571FD1;
	static uint32_t pAddr2 = 0x05722FF;
	static float f = 93.0f;
	__asm
	{
		movss xmm1, f
		comiss xmm0, xmm1
		jg Goto2
		jmp Goto1
		Goto2 :

		jmp[pAddr]

			Goto1 :
			jmp[pAddr2]
	}
}
void __declspec(naked) TitleUI_DisableScrollSound()
{
	static uint32_t pAddr = 0x00572B45;
	__asm
	{
		jmp[pAddr]
	}
}
void __declspec(naked) TitleUI_test()
{
	static uint32_t pAddr = 0x0058C453;
	__asm
	{
		jmp[pAddr]
	}
}void __declspec(naked) TitleUI_test2()
{
	static uint32_t pAddr = 0x0058C69F;
	__asm
	{
		jmp[pAddr]
	}
}
bool inWM = false;
void ContinueToWM()
{
	if (!inWM) {
		Title::SetHideEverything(true);
		TitleWorldMap::Start();
		TitleWorldMap::EnableInput();
		inWM = true;
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
		jne	Options
		jmp[adNewGame]
		jmp	FunctionFinish

		Continue :
		cmp     currentTitleIndex, 1
			jne	Options
			call ContinueToWM
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
			mov isInSubmenu, 1
			jmp[adQuit]

			FunctionFinish :
			jmp[pAddr]
	}
}
void __declspec(naked) TitleUI_SetCustomExecFunctionAdvance()
{
	//https://godbolt.org/
	static uint32_t pAddr = 0x0057372E;
	static uint32_t adNewGame = 0x0057339E;
	static uint32_t adContinue = 0x0057342B;
	static uint32_t adOptions = 0x00573557;
	static uint32_t adQuit = 0x0057364F;
	__asm // NEW: 0| CONTINUE:1 | OPTIONS: 3 | QUIT:  4
	{
		cmp     currentTitleIndex, 0
		jne	Continue
		mov isInSubmenu, 1
		call ExitingTitle
		jmp[adNewGame]
		jmp	FunctionFinish

		Continue :
		cmp     currentTitleIndex, 1
			jne	Options
			call ExitingTitle
			call ContinueToWM
			//jmp[adContinue]
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
			mov isInSubmenu, 1

			FunctionFinish :
			jmp[pAddr]
	}
}
//old but dont remove

//void __declspec(naked) TitleUI_MoveUp()
//{
//	static uint32_t pAddr = 0x010BA693;
//	static uint32_t movement = 0x80000;
//	if (inTitle && scrollHorizontally && !isInSubmenu) //this is global to all of gens for some reason
//		movement = 0x200000;
//	else
//		movement = 0x80000;
//	__asm
//	{
//		test    esi, movement
//		jmp[pAddr]
//	}
//}
//void __declspec(naked) TitleUI_MoveDown()
//{
//	static uint32_t pAddr = 0x010BA6A4;
//	static uint32_t movement = 0x40000;
//	if (inTitle && scrollHorizontally) //this is global to all of gens for some reason
//		movement = 0x100000;
//	else
//		movement = 0x40000;
//	__asm
//	{
//		test    ebx, movement
//		jmp[pAddr]
//	}
//}

void __fastcall CTitleRemoveCallback(Sonic::CGameObject* This, void*, Sonic::CGameDocument* pGameDocument)
{
	Title::KillScreen();
	Chao::CSD::CProject::DestroyScene(rcTitleScreen.Get(), rcTitlebg);
	Chao::CSD::CProject::DestroyScene(rcTitleScreen.Get(), rcTitleLogo_1);
	Chao::CSD::CProject::DestroyScene(rcTitleScreen.Get(), rcTitleMenu);
	Chao::CSD::CProject::DestroyScene(rcTitleScreen.Get(), rcTitleMenuScroll);
	Chao::CSD::CProject::DestroyScene(rcTitleScreen.Get(), rcTitleMenuTXT);
	rcTitleScreen = nullptr;
	enteredStart, isInSubmenu, moved, hasSavefile, playingStartAnim, reversedAnim, isStartAnimComplete, startButtonAnimComplete, startBgAnimComplete = false;
	inTitle, parsedSave = false;
	currentTitleIndex = Title::TitleIndexState::New_Game;
	holdTimer = 0;
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
	{
		pParentGameObject->m_pMember->m_pGameDocument->AddGameObject(spTitleScreen = boost::make_shared<Sonic::CGameObjectCSD>(rcTitleScreen, 0.5f, "HUD", false), "main", pParentGameObject);

		pParentGameObject->SendMessage(pParentGameObject->m_ActorID, boost::make_shared<Sonic::Message::MsgSetGlobalLightDirection>(Hedgehog::math::CQuaternion(1, 1, 1, 1)));
	}
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

void Title::IntroAnim(Chao::CSD::RCPtr<Chao::CSD::CScene> scene)
{
	scene->SetMotion("Intro_Anim");
	scene->SetMotionFrame(0.0f);
	scene->m_MotionSpeed = 1;
	scene->m_MotionRepeatType = Chao::CSD::eMotionRepeatType_PlayOnce;
	scene->Update(0.0f);
}

void UpdateTitleText()
{

	if (currentTitleIndex < 0)
		currentTitleIndex = Title::TitleIndexState::Quit;
	else if (currentTitleIndex > 3)
		currentTitleIndex = Title::TitleIndexState::New_Game;

	if (!rcTitleMenuTXT)
		return;

	rcTitleMenuTXT->SetPosition(0, 0);
	rcTitleMenuTXT->GetNode("txt_0")->SetHideFlag(!(currentTitleIndex == 0));
	rcTitleMenuTXT->GetNode("txt_1")->SetHideFlag(!(currentTitleIndex == 1));
	rcTitleMenuTXT->GetNode("txt_2")->SetHideFlag(!(currentTitleIndex == 2));
	rcTitleMenuTXT->GetNode("txt_3")->SetHideFlag(!(currentTitleIndex == 3));
	rcTitleMenuTXT->GetNode("txt_4")->SetHideFlag(!(currentTitleIndex == 4));
}
HOOK(void, __fastcall, Title_CMain_CState_SelectMenuBegin, 0x572750, hh::fnd::CStateMachineBase::CStateBase* This)
{
	/*originalTitle_CMain_CState_SelectMenuBegin(This);*/
	isInSubmenu = false;
	if (!parsedSave)
	{
		uint32_t owner = (uint32_t)(This->GetContextBase());
		hasSavefile = *(bool*)(owner + 0x1AC);
		currentTitleIndex = hasSavefile ? Title::TitleIndexState::Continue : Title::TitleIndexState::New_Game;
		UpdateTitleText();
		parsedSave = true;
	}
}
void MakeTitleLight()
{
	auto gameDocument = Sonic::CGameDocument::GetInstance();
	auto pos = Hedgehog::Math::CVector(-0.5173238, -0.8544828, -0.0472794);
	auto rcLight = boost::make_shared<Hedgehog::Mirage::CLightData>();
	rcLight->m_Color = { 0.5976471f, 0.5835295f,0.5364707f, 1 };
	rcLight->m_Position = pos;
	rcLight->m_Type = Hedgehog::Mirage::ELightType::eLightType_Point;
	gameDocument->m_pMember->m_spLightManager->AddPointLight(rcLight);
}
HOOK(int, __fastcall, Title_CMain, 0x0056FBE0, Sonic::CGameObject* This, void* Edx, int a2, int a3, void** a4)
{
	CTitleRemoveCallback(This, nullptr, nullptr);
	Title::SetScrollDirection(true);
	//MakeTitleLight();
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
		rcTitleLogo_1 = rcTitleScreen->CreateScene("title_3");
		break;
	}
	}

	rcTitlebg = rcTitleScreen->CreateScene("bg");
	rcTitleMenu = rcTitleScreen->CreateScene("menu");
	rcTitleMenuScroll = rcTitleScreen->CreateScene("menu_scroll");
	rcTitleMenuTXT = rcTitleScreen->CreateScene("txt");
	if (rcTitleMenuTXT)
	{
		rcTitleMenuTXT->GetNode("txt_4")->SetHideFlag(true);
		CSDCommon::FreezeMotion(*rcTitleMenuTXT);
		rcTitleMenuTXT->SetPosition(0, 650000);
	}
	if (rcTitleMenuScroll)
	{
		CSDCommon::PlayAnimation(*rcTitleMenuScroll, "Scroll_Anim_2_1", Chao::CSD::eMotionRepeatType_PlayOnce, 0, 0);
		CSDCommon::FreezeMotion(*rcTitleMenuScroll, 0);
		rcTitleMenuScroll->SetPosition(0, 650000);
	}
	if (rcTitleMenu)
	{
		CSDCommon::FreezeMotion(*rcTitleMenu);
		CSDCommon::PlayAnimation(*rcTitleMenu, "Intro_Anim_1", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	}
	if (rcTitlebg)
	{
		CSDCommon::FreezeMotion(*rcTitlebg);
		CSDCommon::PlayAnimation(*rcTitlebg, "Intro_Anim_1", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	}
	if (rcTitleLogo_1)
		CSDCommon::PlayAnimation(*rcTitleLogo_1, "Intro_Anim_1", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);

	playingStartAnim = true;
	Title::CreateScreen(This);

	return originalTitle_CMain(This, Edx, a2, a3, a4);
}

HOOK(DWORD, *__cdecl, Title_CMain_CState_SelectMenu, 0x11D1210, hh::fnd::CStateMachineBase::CStateBase* This)
{
	if (enteredStart)
		return originalTitle_CMain_CState_SelectMenu(This);
	TitleWorldMap::PlayPanningAnim();
	if (rcTitleMenu)
		CSDCommon::FreezeMotion(*rcTitleMenu);
	if (rcTitlebg)
		CSDCommon::PlayAnimation(*rcTitlebg, "Intro_Anim_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	if (rcTitleMenu)
		CSDCommon::PlayAnimation(*rcTitleMenu, "Intro_Anim_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	if (rcTitleMenuTXT)
	{
		CSDCommon::PlayAnimation(*rcTitleMenuTXT, "Intro_Anim_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		rcTitleMenuTXT->SetPosition(0, 0);
	}
	if (rcTitleMenuScroll)
		rcTitleMenuScroll->SetPosition(0, 0);

	enteredStart = true;
	UpdateTitleText();

	return originalTitle_CMain_CState_SelectMenu(This);
}

HOOK(int, __fastcall, Title_CMain_CState_WaitStart, 0x11D1410, int a1)
{
	if (enteredStart && rcTitleMenu && rcTitlebg)
	{
		CSDCommon::PlayAnimation(*rcTitleMenu, "Intro_Anim_1", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, false, true);
		CSDCommon::PlayAnimation(*rcTitlebg, "Intro_Anim_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 30, 100, false, true);
		currentTitleIndex = Title::TitleIndexState::New_Game;
	}
	if (rcTitleMenuTXT)
		rcTitleMenuTXT->SetPosition(0, 65000);
	if (rcTitleMenuScroll)
		rcTitleMenuScroll->SetPosition(0, 65000);
	reversedAnim = true;
	playingStartAnim = true;
	isStartAnimComplete = false;
	inTitle = true;
	enteredStart = false;
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

HOOK(void*, __fastcall, Title_UpdateApplication, 0xE7BED0, Sonic::CGameObject* This, void* Edx, float elapsedTime, uint8_t a3)
{
	auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->m_CurrentPadStateIndex];


	if (inTitle)
	{

		if (currentTitleIndex == 3 && isInSubmenu && inputPtr->IsDown(Sonic::eKeyState_B)) //janky way of knowing when the quit prompt is aborted
			isInSubmenu = false;

		if (rcTitleLogo_1) //Logo loop anim
		{
			if (rcTitleLogo_1->m_MotionDisableFlag == 1 && !isStartAnimComplete && playingStartAnim)
			{
				isStartAnimComplete = true;
				CSDCommon::PlayAnimation(*rcTitleLogo_1, "Usual_Anim_1", Chao::CSD::eMotionRepeatType_Loop, 1, 0);

			}
		}
		if (rcTitleMenu) //PRESS START loop anim
		{
			if (rcTitleMenu->m_MotionDisableFlag == 1 && !isStartAnimComplete && playingStartAnim)
			{
				CSDCommon::PlayAnimation(*rcTitleMenu, "Usual_Anim_1", Chao::CSD::eMotionRepeatType_Loop, 1, 0);
			}
		}
		if (rcTitlebg && enteredStart) //Pulse header anim
		{
			if (rcTitlebg->m_MotionDisableFlag == 1 && !startBgAnimComplete && playingStartAnim)
			{
				startBgAnimComplete = true;
				CSDCommon::PlayAnimation(*rcTitlebg, "Usual_Anim_2", Chao::CSD::eMotionRepeatType_Loop, 1, 0);
			}
		}
#pragma region UIScroll
		if (rcTitleMenu && !isInSubmenu && enteredStart)
		{
			if (IsRightDown() && !moved)
			{
				currentTitleIndex = (Title::TitleIndexState)((int)(currentTitleIndex)+1);
				if (currentTitleIndex > 3)
					currentTitleIndex = Title::TitleIndexState::New_Game;

				CSDCommon::PlayAnimation(*rcTitleMenuScroll, "Scroll_Anim_2_1", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
				CSDCommon::PlayAnimation(*rcTitleMenu, "Scroll_Anim_2_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 100, false, true);
				UpdateTitleText();
				moved = true;
			}
			if (IsLeftDown() && !moved)
			{
				currentTitleIndex = (Title::TitleIndexState)((int)(currentTitleIndex)-1);
				if (currentTitleIndex < 0 || currentTitleIndex > 3) //uint32s become huge when going negative
					currentTitleIndex = Title::TitleIndexState::Quit;

				CSDCommon::PlayAnimation(*rcTitleMenuScroll, "Scroll_Anim_2_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
				CSDCommon::PlayAnimation(*rcTitleMenu, "Scroll_Anim_2_1", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 100, false, true);
				UpdateTitleText();
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
#pragma endregion
	}
	return originalTitle_UpdateApplication(This, Edx, elapsedTime, a3);
}


HOOK(int, __fastcall, Title_CMain_ExecSubMenu, 0x572D00, DWORD* This)
{
	int returned = 0;
	if (currentTitleIndex > 2 && !isInSubmenu)
	{
		isInSubmenu = true;
		returned = originalTitle_CMain_ExecSubMenu(This);
	}
	else if (currentTitleIndex < 2)
		isInSubmenu = false;

	if (!isInSubmenu && currentTitleIndex != 1)
		return originalTitle_CMain_ExecSubMenu(This);
	else
		return returned; //will 9/10 times return 0

}
HOOK(void, __fastcall, TitleUI_TitleCMainCState_SelectMenuAdvance, 0x5728F0, hh::fnd::CStateMachineBase::CStateBase* This)
{
	uint32_t owner = (uint32_t)(This->GetContextBase());
	Title::OutState = (uint32_t*)(owner + 0x1BC);
	*Title::OutState = 4;

	static SharedPtrTypeless spOutState;
	/*TitleUI_TinyChangeState(This, spOutState, "Finish");*/
	if (!isInSubmenu)
		return originalTitleUI_TitleCMainCState_SelectMenuAdvance(This);
	else
	{
		originalTitleUI_TitleCMainCState_SelectMenuAdvance(This);
		isInSubmenu = true;
	}
}

//FUNCTION_PTR(int, __fastcall, sub_D131B0, 0xD131B0, Hedgehog::math::CVector* This, int a2);
//void Setlightdir() {
//	sub_D131B0(Hedgehog::math::CVector(0,0,0), )
//}



void __declspec(naked) setlightposX()
{
	//https://godbolt.org/
	static uint32_t pAddr = 0x0058D54B;
	static float x = -79.8565f;

	__asm // NEW: 0| CONTINUE:1 | OPTIONS: 3 | QUIT:  4
	{
		movss xmm0, x;
		jmp[pAddr]
	}
}
void __declspec(naked) setlightposY()
{
	//https://godbolt.org/
	static uint32_t pAddr = 0x0058D559;
	static float y = 0;

	__asm // NEW: 0| CONTINUE:1 | OPTIONS: 3 | QUIT:  4
	{
		movss xmm0, y;
		jmp[pAddr]
	}
}void __declspec(naked) setlightposZ()
{
	//https://godbolt.org/
	static uint32_t pAddr = 0x0058D567;
	static float z = 4.78983f;

	__asm // NEW: 0| CONTINUE:1 | OPTIONS: 3 | QUIT:  4
	{
		movss xmm0, z;
		jmp[pAddr]
	}
}
void Title::Install()
{
	//Set up title screen so that it resembles Unleashed function-wise
	WRITE_JUMP(0x00571FCA, TitleUI_SetCutsceneTimer); //Set title AFK wait amount - it varies depending on framerate
	WRITE_JUMP(0x00572D23, TitleUI_SetCustomExecFunction); //Override Button Function
	WRITE_JUMP(0x005732C3, TitleUI_SetCustomExecFunctionAdvance); //Override button after-function
	WRITE_JUMP(0x00572B2E, 0x00572B45); //Disable scroll sound


	WRITE_JUMP(0x0058CE33, 0x0058CEAB);

	WRITE_JUMP(0x0058D543, setlightposX);
	WRITE_JUMP(0x0058D551, setlightposY);
	WRITE_JUMP(0x0058D55F, setlightposZ);
	WRITE_MEMORY(0x1704474, float, 0.4f); //FOV
	Eigen::Vector2f* qa = (Eigen::Vector2f*)0x1A42300;
	qa->x() = -0.8544828f;
	qa->y() = -0.0472794f;


	Eigen::Vector3f* lightColor = (Eigen::Vector3f*)0x01A42308;
	lightColor->x() = 0.5976471f;
	lightColor->y() = 0.5835295f;
	lightColor->z() = 0.5364707f;

	//UI
	INSTALL_HOOK(Title_UpdateApplication);
	WRITE_MEMORY(0x016E11F4, void*, CTitleRemoveCallback);
	WRITE_JUMP(0x572D3A, (void*)0x57326B); // Don't create delete save dialog
	WRITE_NOP(0x00572976, 6); //Always skip waiting for anim in ExecSubMenu
	WRITE_NOP(0x00572930, 6); //Always skip waiting for anim in ExecSubMenu


	//State Hooks
	INSTALL_HOOK(Title_CMain);
	INSTALL_HOOK(Title_CMain_ExecSubMenu);
	INSTALL_HOOK(Title_CMain_CState_SelectMenuBegin);
	INSTALL_HOOK(TitleUI_TitleCMainCState_SelectMenuAdvance);
	INSTALL_HOOK(Title_CMain_CState_WaitStart);
	INSTALL_HOOK(Title_CMain_CState_SelectMenu);
}