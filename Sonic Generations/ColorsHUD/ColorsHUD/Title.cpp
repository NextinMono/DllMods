Chao::CSD::RCPtr<Chao::CSD::CProject> rcTitleScreen;
Chao::CSD::RCPtr<Chao::CSD::CScene> title;
boost::shared_ptr<Sonic::CGameObjectCSD> spTitleScreen;
int currentTitleIndex = 0;
int timePassed = 1;
bool entered = false;
bool movedT = false;
int movementTest = 0;
int movementTestPrev = 5;
bool startAnimComplete, startButtonAnimComplete, startBgAnimComplete = false;
bool hasSavefile = false;
Title::TState State = Title::TState::PressStartIntro;

inline FUNCTION_PTR(int, __fastcall, CTitleMainFinishH, 0x5727F0, void* Edx, Sonic::CGameObject* This);

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
void __declspec(naked) TitleUI_TitleScroll()
{
	static uint32_t pAddr = 0x00572837;
	__asm
	{		
		mov edi, -1
		jmp [pAddr]
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

void ColorsSelect()
{	
	if (currentTitleIndex < 0)
		currentTitleIndex = 3;
	else if (currentTitleIndex > 3)
		currentTitleIndex = 0;
	char integer_string[32];
	sprintf(integer_string, "Usual_Anim_%d", currentTitleIndex);
	CSDCommon::PlayAnimation(*title, integer_string, Chao::CSD::eMotionRepeatType_Loop, 1, 0);

	printf("\nCURRENT INDEX: %d", currentTitleIndex);
	
	/*Title::PlayAnimation(*rcTitleMenuTXT, "Intro_Anim_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);*/
}
HOOK(void, __fastcall, TitleUI_TitleCMainCState_SelectMenuBegin, 0x572750, hh::fnd::CStateMachineBase::CStateBase* This)
{
	uint32_t owner = (uint32_t)(This->GetContextBase());
	hasSavefile = *(bool*)(owner + 0x1AC);
	currentTitleIndex = hasSavefile ? 1 : 0;
	ColorsSelect();
}
HOOK(int, __fastcall, CTitleMain, 0x0056FBE0, Sonic::CGameObject* This, void* Edx, int a2, int a3, void** a4)
{
	rcTitleScreen = nullptr;
	originalCTitleMain(This, Edx, a2, a3, a4);
	rcTitleScreen = nullptr;

	Sonic::CCsdDatabaseWrapper wrapper(This->m_pMember->m_pGameDocument->m_pMember->m_spDatabase.get());

	auto spCsdProject = wrapper.GetCsdProject("ui_title_colors");
	rcTitleScreen = spCsdProject->m_rcProject;

	title = rcTitleScreen->CreateScene("title");
	CSDCommon::PlayAnimation(*title, "main_Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);

	Title::CreateScreen(This);

	return 0;
}

HOOK(int, __fastcall, CTitleMainSelect, 0x11D1210, int a1)
{
	State = Title::TState::ButtonsGeneralIntro;
	CSDCommon::PlayAnimation(*title, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	entered = true;
	ColorsSelect();

	return originalCTitleMainSelect(a1);
}

HOOK(int, __fastcall, CTitleMainWait, 0x11D1410, int a1)
{/*
	CSDCommon::PlayAnimation(*title, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, false, true);*/
	State = Title::TState::PressStartUsual;
	return originalCTitleMainWait(a1);
}

HOOK(int, __fastcall, CTitleMainFinish, 0x5727F0, void* Edx, void* This)
{
	int aaa = (int)This;
	printf("%d", aaa);
	originalCTitleMainFinish(Edx, This);
	ColorsSelect();
	return 0;
}
float m_applicationDeltaTimeT = 0.0f;
HOOK(void*, __fastcall, Title_UpdateApplication, 0xE7BED0, void* This, void* Edx, float elapsedTime, uint8_t a3)
{
	m_applicationDeltaTimeT = elapsedTime;
	timePassed += 1;
	//printf("\nDeltaTime: %d", m_applicationDeltaTimeT);
	//printf("| TimePassed: %d", timePassed);/*
	/*FUNCTION_PTR(int, __cdecl, FirstSetup, 0x11D10A0, int);*/
	if (title)
	{
		switch (State)
		{
		case Title::ButtonsGeneralOutro:
		{
			if (title->m_MotionDisableFlag == 1)
			{
				CSDCommon::PlayAnimation(*title, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, false, true);
			}
			if (title->m_MotionFrame <= 0)
			{
				State = Title::PressStartIntro;
			}
		}		
		case Title::PressStartUsual:
		{
			if (title->m_MotionDisableFlag == 1)
			{
				CSDCommon::PlayAnimation(*title, "main_Usual_Anim", Chao::CSD::eMotionRepeatType_Loop, 1, 0, 0, false, true);
			}
		}
		}

		
		printf("Bool: %d",movedT);

	}
	auto inputState = Sonic::CInputState::GetInstance();
	auto inputPtr = &inputState->m_PadStates[inputState->m_CurrentPadStateIndex];
	movementTestPrev = movementTest;
	if (inputPtr->LeftStickHorizontal >= 0.8f && !movedT)
	{
		movementTest = -1;
		movedT = true;
	}
	if (inputPtr->LeftStickHorizontal < -0.8f && !movedT)
	{
		movementTest = 1;
		movedT = true;
	}
	if (inputPtr->LeftStickHorizontal == 0)
	{
		movedT = false;
		movementTest = 0;
	}
	printf("\nMovement: %d |", movementTest);
	printf("MovementP: %d", movementTestPrev);

	return originalTitle_UpdateApplication(This, Edx, elapsedTime, a3);
}
HOOK(void, __fastcall, titlestatemachinemaybe, 0x571F80, int This, void* Edx)
{
	originaltitlestatemachinemaybe(This, Edx);
	DWORD e = *(DWORD*)(This + 36);
	printf("%lu", e);
	switch (e)
	{
	case 0:
	{
		printf("case 0");
		break;
	}
	case 3:
	{
		printf("case 3");
		break;
	}
	case 4: {
		printf("case 4");
		break;
	}
	case 5: {
		printf("case 5");
		break;
	}
	case 6: {
		printf("case 6");
		break;
	}
	case 7: {
		printf("case 7");
		break;
	}
	default:
	{
		break;
	}
	}
}
HOOK(void, __fastcall,Test, 0x107EBD0, int* This, void* Edx)
{
	
	originalTest(This, Edx);
}
void __declspec(naked) TitleUI_TitleScrollT()
{
	static uint32_t pAddr = 0x00572A41;
	__asm
	{
		mov edi, movementTest
		mov ecx, movementTestPrev
		jmp[pAddr]
	}
}
void __declspec(naked) TitleUI_TitleScrollB()
{
	static uint32_t pAddr = 0x5729EF;
	static uint32_t pAddrT = 0x005729AF;
	__asm
	{
		cmp movementTest, 1
		jmp[pAddr]
	}
}
HOOK(void, __fastcall, test, 0x5728F0, int This, void* Edx)
{
	
	originaltest(This, Edx);
}
void Title::Install()
{
	/*WRITE_JUMP(0x005727FC, TitleUI_TitleScroll);*/
	//WRITE_JUMP(0x5729EF, TitleUI_TitleScrollT);/*
	/*WRITE_JUMP(0x005729AF, TitleUI_TitleScrollB);*/
	/*INSTALL_HOOK(test);*/
	///*INSTALL_HOOK(titlestatemachinemaybe);*/
	INSTALL_HOOK(TitleUI_TitleCMainCState_SelectMenuBegin);
	INSTALL_HOOK(CTitleMain);
	INSTALL_HOOK(Title_UpdateApplication);
	INSTALL_HOOK(CTitleMainFinish);
	INSTALL_HOOK(CTitleMainSelect);

	INSTALL_HOOK(CTitleMainWait);
}