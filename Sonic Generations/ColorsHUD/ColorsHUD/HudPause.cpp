
Chao::CSD::RCPtr<Chao::CSD::CProject> projPause;
boost::shared_ptr<Sonic::CGameObjectCSD> spPause;
Chao::CSD::RCPtr<Chao::CSD::CScene> pause_window;

Chao::CSD::RCPtr<Chao::CSD::CProject> projWindowUniversal;
boost::shared_ptr<Sonic::CGameObjectCSD> spWindowUniversal;
Chao::CSD::RCPtr<Chao::CSD::CScene> universal_window;
int cursorOptionsSize = 8;
Chao::CSD::RCPtr<Chao::CSD::CScene> cursors_Options[8] = {};

uint32_t m_cursorPos = 0;
bool closePause = false;
bool pauseWindowScanline = false;

void SetCursorPos(int pairIndex, Hedgehog::Math::CVector2 pos)
{
	float offset = 100;
	if (pairIndex == 0)
	{
		cursors_Options[0]->GetNode("position")->SetPosition(pos.x() - offset, pos.y());
		cursors_Options[1]->GetNode("position")->SetPosition(pos.x() + offset, pos.y());
	}
	if (pairIndex == 1)
	{
		cursors_Options[2]->GetNode("position")->SetPosition(pos.x() - offset, pos.y());
		cursors_Options[3]->GetNode("position")->SetPosition(pos.x() + offset, pos.y());
	}
	if (pairIndex == 2)
	{
		cursors_Options[4]->GetNode("position")->SetPosition(pos.x() - offset, pos.y());
		cursors_Options[5]->GetNode("position")->SetPosition(pos.x() + offset, pos.y());
	}
	if (pairIndex == 3)
	{
		cursors_Options[6]->GetNode("position")->SetPosition(pos.x() - offset, pos.y());
		cursors_Options[7]->GetNode("position")->SetPosition(pos.x() + offset, pos.y());
	}
}
void PlayCursorAnimation(const char* animationName, float frame = 0, bool oneFrame = false, bool reverse = false)
{
	float endFrame = cursors_Options[0]->m_MotionEndFrame;
	if (oneFrame)
		endFrame = frame;
	for (size_t i = 0; i < cursorOptionsSize; i++)
	{
		CSDCommon::PlayAnimation(*cursors_Options[i], animationName, Chao::CSD::eMotionRepeatType_PlayOnce, 1, frame, endFrame + 0.1f, false, reverse);
	}
}
void PlayCursorPairAnimation(const char* animationName, int pairIndex)
{
	if (pairIndex == 0)
	{
		CSDCommon::PlayAnimation(*cursors_Options[0], animationName, Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		CSDCommon::PlayAnimation(*cursors_Options[1], animationName, Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	}
	if (pairIndex == 1)
	{
		CSDCommon::PlayAnimation(*cursors_Options[2], animationName, Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		CSDCommon::PlayAnimation(*cursors_Options[3], animationName, Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	}
	if (pairIndex == 2)
	{
		CSDCommon::PlayAnimation(*cursors_Options[4], animationName, Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		CSDCommon::PlayAnimation(*cursors_Options[5], animationName, Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	}
	if (pairIndex == 3)
	{
		CSDCommon::PlayAnimation(*cursors_Options[6], animationName, Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		CSDCommon::PlayAnimation(*cursors_Options[7], animationName, Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	}
}

void SetHideFlagCursor(bool hidden)
{
	for (size_t i = 0; i < cursorOptionsSize; i++)
	{
		cursors_Options[i]->SetHideFlag(hidden);
		cursors_Options[i]->m_MotionDisableFlag = 1;
	}
}
void HudPause_PlayMotion(Chao::CSD::RCPtr<Chao::CSD::CScene>& scene, char const* motion, bool loop = false, float start = 0.0f, float end = -1.0f, bool reverse = false)
{
	float speed = 1;
	if (!scene) return;
	scene->SetHideFlag(false);
	scene->SetMotion(motion);
	if (reverse)
	{
		float temp = start;
		start = end;
		end = temp;
		speed = -speed;
	}
	scene->SetMotionFrame(start);
	if (end > start)
	{
		scene->m_MotionEndFrame = end;
	}
	scene->m_MotionDisableFlag = false;
	scene->m_MotionSpeed = 1.0f;
	scene->m_MotionRepeatType = loop ? Chao::CSD::eMotionRepeatType_Loop : Chao::CSD::eMotionRepeatType_PlayOnce;
	scene->Update();
}


void HudPause_StopMotion(Chao::CSD::RCPtr<Chao::CSD::CScene>& scene, char const* motion, float frame)
{
	if (!scene) return;
	scene->SetHideFlag(false);
	scene->SetMotion(motion);
	scene->SetMotionFrame(frame);
	scene->m_MotionDisableFlag = true;
	scene->m_MotionSpeed = 0.0f;
	scene->m_MotionRepeatType = Chao::CSD::eMotionRepeatType_PlayOnce;
	scene->Update();
}

#pragma region Pause
void __fastcall HudPause_CPauseRemoveCallback(Sonic::CGameObject* This, void*, Sonic::CGameDocument* pGameDocument)
{
	if (spPause)
	{
		spPause->SendMessage(spPause->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
		spPause = nullptr;
	}

	//Chao::CSD::CProject::DestroyScene(projPause.Get(), m_scenePauseBG);
	//Chao::CSD::CProject::DestroyScene(projPause.Get(), m_scenePauseHeader);
	Chao::CSD::CProject::DestroyScene(projPause.Get(), pause_window);
	for (size_t i = 0; i < cursorOptionsSize; i++)
	{
		Chao::CSD::CProject::DestroyScene(projPause.Get(), cursors_Options[i]);
	}

	projPause = nullptr;
}
//test

void HudPause_CreatePauseScreen(uint32_t* This)
{
	Sonic::CGameObject* gameObject = (Sonic::CGameObject*)This[1];
	HudPause_CPauseRemoveCallback(gameObject, nullptr, nullptr);
	

	Sonic::CCsdDatabaseWrapper wrapper(gameObject->m_pMember->m_pGameDocument->m_pMember->m_spDatabase.get());
	projPause = wrapper.GetCsdProject("ui_pause_colors")->m_rcProject;

	pause_window = projPause->CreateScene("pause");
	for (size_t i = 0; i < cursorOptionsSize; i++)
	{
		cursors_Options[i] = projPause->CreateScene("cursor");
		
	}
	Hedgehog::Math::CVector2 offscreen = Hedgehog::Math::CVector2(-100, -90);
	SetCursorPos(0, offscreen);
	SetCursorPos(1, offscreen);
	SetCursorPos(2, offscreen);
	SetCursorPos(3, offscreen);
	pause_window->SetHideFlag(true);
	pause_window->GetNode("word_1")->SetHideFlag(true);
	pause_window->GetNode("word_1")->SetHideFlag(true);
	pause_window->GetNode("word_1")->SetHideFlag(true);
	pause_window->GetNode("word_2")->SetHideFlag(true);
	pause_window->GetNode("word_3")->SetHideFlag(true);
	if (projPause && !spPause)
	{
		spPause = boost::make_shared<Sonic::CGameObjectCSD>(projPause, 0.5f, "HUD_A2", true);
		Sonic::CGameDocument::GetInstance()->AddGameObject(spPause, "main", gameObject);
	}
}

void HudPause_OpenPauseWindow(bool isPam)
{
	m_cursorPos = 0;

	if (!projPause) return;
	//pause_window->GetNode("center")->SetScale(62.0f, isPam ? 0.5f : 0.9f);
	//pause_window->GetNode("text_area")->SetScale(62.0f, isPam ? 0.5f : 0.9f);
	HudPause_PlayMotion(pause_window, "Intro_Anim");
	SetHideFlagCursor(false);
	PlayCursorAnimation("Intro_Anim");
}

void HudPause_OpenPauseScreen(bool isPam)
{
	if (!projPause) return;
	/*HudPause_PlayMotion(m_scenePauseBG, "Intro_Anim");
	HudPause_PlayMotion(m_scenePauseHeader, "Intro_Anim");*/
	HudPause_PlayMotion(pause_window, "Size_Anim", false, 20, 21);
	/*pause_select->GetNode("img")->SetScale(38.0f, 1.0f);*/
	closePause = false;
	SetHideFlagCursor(false);
	HudPause_OpenPauseWindow(isPam);
}

void HudPause_ClosePauseScreen()
{
	if (!projPause) return;
	//m_scenePauseBG->SetHideFlag(true);
	//m_scenePauseHeader->SetHideFlag(true);
	pause_window->SetMotion("Intro_Anim");
	CSDCommon::PlayAnimation(*pause_window, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 5, pause_window->m_MotionEndFrame, false, true);

	
	closePause = true;
	/*HudPause_PlayMotion(pause_window, "Intro_Anim", false, 0, pause_window->m_MotionEndFrame, true);*/	
}

void HudPause_PauseCase(uint32_t* This, int Case, bool isPam)
{
	if (!projPause) return;

	uint32_t cursorPos = This[3];
	auto& originalScene = *(Chao::CSD::RCPtr<Chao::CSD::CScene>*)(&This[9]);
	if (!originalScene)
		return;
	switch (Case)
	{
	case 0: // Start
	{
		originalScene->SetHideFlag(true);

		HudPause_OpenPauseScreen(isPam);
		if (!isPam)
		{
			originalScene->GetNode("continue_sdw")->SetPosition(0, -80);
			originalScene->GetNode("restart_sdw")->SetPosition(0, -30);
			originalScene->GetNode("how_to_control_sdw")->SetPosition(0, 20);
			originalScene->GetNode("quit_sdw")->SetPosition(0, 73);
			originalScene->GetNode("Null_text")->SetPosition(0, 2);

			//PlayCursorAnimation("Intro_Anim");
		}
		else
		{
			originalScene->GetNode("position")->SetScale(0.65f, 0.65f);
			originalScene->SetPosition(2, 35);
		}
		break;
	}
	case 1: // End
	{
		/*CSDCommon::PlayAnimation(*originalScene, "select_Anim_1", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);*/
		originalScene->SetHideFlag(true);			
		break;
	}
	case 2: // Start animation complete
	{
		originalScene->SetHideFlag(false);
		if (!isPam)
		{
			Hedgehog::Math::CVector2 cp = originalScene->GetNode("continue_sdw")->GetPosition();
			Hedgehog::Math::CVector2 rp = originalScene->GetNode("restart_sdw")->GetPosition();
			Hedgehog::Math::CVector2 hp = originalScene->GetNode("how_to_control_sdw")->GetPosition();
			Hedgehog::Math::CVector2 qp = originalScene->GetNode("quit_sdw")->GetPosition();


			PlayCursorAnimation("Intro_Anim", 10, true);
			SetCursorPos(0, cp);
			SetCursorPos(1, rp);
			SetCursorPos(2, hp);
			SetCursorPos(3, qp);
		}
		else
		{
			originalScene->SetMotionFrame(originalScene->m_MotionEndFrame);
			Hedgehog::Math::CVector2 cp = originalScene->GetNode("2_1")->GetPosition();
			Hedgehog::Math::CVector2 rp = originalScene->GetNode("2_2")->GetPosition();
			Hedgehog::Math::CVector2 ip = Hedgehog::Math::CVector2(909, 909);
			cp.y() += -5;
			rp.y() += -5;


			PlayCursorAnimation("Intro_Anim", 10, true);
			SetCursorPos(0, cp);
			SetCursorPos(1, rp);
			SetCursorPos(2, ip);
			SetCursorPos(3, ip);
		}
		
		PlayCursorPairAnimation("Active_Anim_L", cursorPos);
		HudPause_PlayMotion(pause_window, "Usual_Anim", true, 0);
		break;
	}
	case 3: // New cursor pos
	{
		originalScene->SetMotionFrame(originalScene->m_MotionEndFrame);

		if (!isPam)
		{
			auto cp = originalScene->GetNode("continue_sdw")->GetPosition();
			auto rp = originalScene->GetNode("restart_sdw")->GetPosition();
			auto hp = originalScene->GetNode("how_to_control_sdw")->GetPosition();
			auto qp = originalScene->GetNode("quit_sdw")->GetPosition();

			SetCursorPos(0, cp);
			SetCursorPos(1, rp);
			SetCursorPos(2, hp);
			SetCursorPos(3, qp);
		}
		else
		{
			Hedgehog::Math::CVector2 cp = originalScene->GetNode("2_1")->GetPosition();
			Hedgehog::Math::CVector2 rp = originalScene->GetNode("2_2")->GetPosition();
			Hedgehog::Math::CVector2 ip = Hedgehog::Math::CVector2(909, 909);
			cp.y() += -5;
			rp.y() += -5;

			SetCursorPos(0, cp);
			SetCursorPos(1, rp);
			SetCursorPos(2, ip);
			SetCursorPos(3, ip);

		}


		PlayCursorAnimation("Intro_Anim", 10, true);
		PlayCursorPairAnimation("Active_Anim_L", cursorPos);

		break;
	}
	case 4: // Confirm
	{
		pause_window->m_MotionDisableFlag = 1;
		closePause = true;
		break;
	}
	case 5: // Back from confirm dialog
	{
		closePause = false;
		originalScene->SetHideFlag(false);
		HudPause_OpenPauseWindow(isPam);
		break;
	}
	default:
	{
		break; }

	}

	m_cursorPos = cursorPos;
}


HOOK(bool, __fastcall, HudPause_CPauseVisualActInit, 0x109FAD0, uint32_t* This)
{
	HudPause_CreatePauseScreen(This);
	return originalHudPause_CPauseVisualActInit(This);
}

HOOK(bool, __fastcall, HudPause_CPauseVisualActOpened, 0x109F8F0, uint32_t* This)
{	
	return pause_window->m_MotionDisableFlag == 1;
}

HOOK(int, __fastcall, HudPause_CPauseVisualActCase, 0x109F910, uint32_t* This, void* Edx, int Case)
{
	int result = originalHudPause_CPauseVisualActCase(This, Edx, Case);
	HudPause_PauseCase(This, Case, false);
	return result;
}


HOOK(void, __fastcall, HudPause_CPauseCStateCloseBegin, 0x42A710, hh::fnd::CStateMachineBase::CStateBase* This)
{
	int* context = (int*)This->m_pContext;
	if (context[58] != -1)
	{
		// Unpause
		HudPause_ClosePauseScreen();
	}
	else
	{
		// HOWTO/WINDOW
		if (pause_window)
		{
			CSDCommon::PlayAnimation(*pause_window, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 5, pause_window->m_MotionEndFrame, false, true);			
		}
	}

	originalHudPause_CPauseCStateCloseBegin(This);
}

HOOK(int, __fastcall, HudPause_CPauseCStateIdleBegin, 0x42A340, hh::fnd::CStateMachineBase::CStateBase* This)
{
	HudPause_ClosePauseScreen();
	return originalHudPause_CPauseCStateIdleBegin(This);
}

HOOK(bool, __fastcall, HudPause_CPauseVisualPamInit, 0x109E9D0, uint32_t* This)
{
	HudPause_CreatePauseScreen(This);
	if (pause_window)
	{
		pause_window->GetNode("position")->SetScale(1, 0.65f);
		pause_window->GetNode("position")->SetPosition(1280/2, 720/2-5);
	}
	return originalHudPause_CPauseVisualPamInit(This);;
}

HOOK(bool, __fastcall, HudPause_CPauseVisualPamOpened, 0x109E720, uint32_t* This)
{
	return pause_window->m_MotionDisableFlag == 1;
}

HOOK(int, __fastcall, HudPause_CPauseVisualPamCase, 0x109E740, uint32_t* This, void* Edx, int Case)
{
	int result = Case == 2 ? 0 : originalHudPause_CPauseVisualPamCase(This, Edx, Case);
	HudPause_PauseCase(This, Case, true);
	return result;
}
#pragma endregion
#pragma region Window_Universal

void __fastcall HudPause_CWindowImplRemoveCallback(Sonic::CGameObject* This, void*, Sonic::CGameDocument* pGameDocument)
{
	if (spWindowUniversal)
	{
		spWindowUniversal->SendMessage(spWindowUniversal->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
		spWindowUniversal = nullptr;
	}

	Chao::CSD::CProject::DestroyScene(projWindowUniversal.Get(), universal_window);

	projWindowUniversal = nullptr;
}

void HudPause_CreateWindowScreen(Sonic::CGameObject* gameObject)
{
	HudPause_CWindowImplRemoveCallback(gameObject, nullptr, nullptr);

	Sonic::CCsdDatabaseWrapper wrapper(gameObject->m_pMember->m_pGameDocument->m_pMember->m_spDatabase.get());
	projWindowUniversal = wrapper.GetCsdProject("ui_universal")->m_rcProject;

	universal_window = projWindowUniversal->CreateScene("universal");
	universal_window->SetHideFlag(true);
	universal_window->SetPosition(1280 / 2, 720 / 2);
	if (projWindowUniversal && !spWindowUniversal)
	{
		spWindowUniversal = boost::make_shared<Sonic::CGameObjectCSD>(projWindowUniversal, 0.5f, "HUD_A2", true);
		Sonic::CGameDocument::GetInstance()->AddGameObject(spWindowUniversal, "main", gameObject);
	}
}
void SetWindowSize(float x, float y)
{
	Hedgehog::Math::CVector2 sxy_vc, line_size, sxy, sx_zero, sx_one, sx_two, sx_three, word, sy;
	universal_window->SetMotion("Size_Anim_X");
	universal_window->m_MotionDisableFlag = 0;
	universal_window->SetMotionFrame(x);
	universal_window->m_MotionSpeed = 0;
	universal_window->Update(0);/*
	sxy_vc.y = CSDCommon::lerp(0, 77, y);*/


}
HOOK(int, __fastcall, HudPause_CWindowImplCStateOpenBegin, 0x439120, hh::fnd::CStateMachineBase::CStateBase* This)
{
	Sonic::CGameObject* parent = (Sonic::CGameObject*)(This->GetContextBase());
	HudPause_CreateWindowScreen(parent);

	float* context = (float*)(This->GetContextBase());
	float widthMotionRatio = context[242] / 1.65f; /** 0.01f * 0.8f;*/
	float heightMotionRatio = context[243] / 1.65f;

	/*universal_window->GetNode("bg")->SetHideFlag(true);*/
	CSDCommon::PlayAnimation(*universal_window, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	universal_window->SetHideFlag(false);
	universal_window->GetNode("sxy_vc")->SetScale(widthMotionRatio / 12, heightMotionRatio * 1.5f);
	universal_window->GetNode("sy")->SetScale(widthMotionRatio / 12.1f, heightMotionRatio * 1.5f);
	universal_window->GetNode("sxy")->SetScale(widthMotionRatio / 12.1f, heightMotionRatio * 1.5f);
	universal_window->GetNode("sx_0")->SetScale((widthMotionRatio / 4 + (0.018f * context[242])) - 1.72f, 1);
	universal_window->GetNode("sx_1")->SetScale((widthMotionRatio / 4 + (0.018f * context[242])) - 1.72f, 1);
	universal_window->GetNode("sx_2")->SetScale((widthMotionRatio / 4 + (0.018f * context[242])) - 1.72f, 1);
	universal_window->GetNode("sx_3")->SetScale((widthMotionRatio / 4 + (0.018f * context[242])) - 1.72f, 1);
	return originalHudPause_CWindowImplCStateOpenBegin(This);
}

HOOK(void*, __fastcall, HudPause_UpdateApplication, 0xE7BED0, void* This, void* Edx, float elapsedTime, uint8_t a3)
{
	if (pause_window)
	{
		if (closePause == true)
		{
			if (pause_window->m_MotionFrame <= 0)
			{
				pause_window->SetHideFlag(true);
				pause_window->m_MotionDisableFlag = 1;
			}
		}
	}
	if(cursors_Options[1])
	{
		if (closePause)
		{	
			SetHideFlagCursor(true);
		}
	}
	return originalHudPause_UpdateApplication(This, Edx, elapsedTime, a3);
}

HOOK(int, __fastcall, HudPause_CWindowImplCStateCloseBegin, 0x4395A0, hh::fnd::CStateMachineBase::CStateBase* This)
{
	universal_window->SetHideFlag(true);
	return originalHudPause_CWindowImplCStateCloseBegin(This);
}
#pragma endregion
void Install_Window()
{
	//---------------------------------------------------
	// CWindowImpl
	//---------------------------------------------------
	WRITE_MEMORY(0x16A6D84, void*, HudPause_CWindowImplRemoveCallback);
	INSTALL_HOOK(HudPause_CWindowImplCStateOpenBegin);
	INSTALL_HOOK(HudPause_CWindowImplCStateCloseBegin);
}
void Install_Pause()
{
	//---------------------------------------------------
	// CPause
	//---------------------------------------------------
	WRITE_MEMORY(0x16A41A4, void*, HudPause_CPauseRemoveCallback);

	INSTALL_HOOK(HudPause_CPauseVisualActInit);
	INSTALL_HOOK(HudPause_CPauseVisualActOpened);
	INSTALL_HOOK(HudPause_CPauseVisualActCase);

	INSTALL_HOOK(HudPause_CPauseVisualPamInit);
	INSTALL_HOOK(HudPause_CPauseVisualPamOpened);
	INSTALL_HOOK(HudPause_CPauseVisualPamCase);

	INSTALL_HOOK(HudPause_CPauseCStateCloseBegin);
	INSTALL_HOOK(HudPause_CPauseCStateIdleBegin);

	// Put HowTo layout to HUD_Pause
	WRITE_MEMORY(0x10AFC56, uint32_t, 0x1E66C18);

	// Don't show mission objective at pause
	WRITE_MEMORY(0xD00A46, uint8_t, 0);
	WRITE_MEMORY(0xD07489, uint8_t, 0);
}
void HudPause::Install()
{
	INSTALL_HOOK(HudPause_UpdateApplication);
	//// PAM pause sfx
	//WRITE_MEMORY(0x11D203A, uint32_t, 1000000);
	//WRITE_MEMORY(0x11D1F8A, uint32_t, 1000001);
	Install_Pause();
	Install_Window();
}
