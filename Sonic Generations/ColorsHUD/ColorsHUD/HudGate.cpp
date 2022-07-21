
Chao::CSD::RCPtr<Chao::CSD::CProject> projGate;
boost::shared_ptr<Sonic::CGameObjectCSD> spGate;
Chao::CSD::RCPtr<Chao::CSD::CScene> colors_base, colors_act_name, colors_score, colors_stageContainer, og_Main, og_Deco;
bool switching;
bool isClosing;
bool classic = false;


void HudGate::CreateScreen(Sonic::CGameObject* pParentGameObject)
{	
	if (projGate && !spGate)
		pParentGameObject->m_pMember->m_pGameDocument->AddGameObject(spGate = boost::make_shared<Sonic::CGameObjectCSD>(projGate, 0.5f, "HUD", false), "main", pParentGameObject);
}

void HudGate::KillScreen()
{
	if (spGate)
	{
		spGate->SendMessage(spGate->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
		spGate = nullptr;
	}
}

void HudGate::ToggleScreen(const bool visible, Sonic::CGameObject* pParentGameObject)
{
	if (visible)
	{
		HudGate::CreateScreen(pParentGameObject);
	}
	else
		HudGate::KillScreen();
}
HOOK(void, __fastcall, CHudGateMenuMainIntro, 0x10804C0, hh::fnd::CStateMachineBase::CStateBase This, void* Edx)
{
	//Ideally, this would load a separate XNCP. but considering its not a pointer, idk how i'd do that.
	printf("I");
	originalCHudGateMenuMainIntro(This, Edx);
}
//Saying "outro" is a bit misleading, but it is essentially a state check to load info, like actinfo or informationpod
HOOK(int, __fastcall, CHudGateMenuMainOutro, 0x1080C30, hh::fnd::CStateMachineBase::CStateBase* This, void* Edx)
{
	int* context = (int*)This->GetContextBase();
	byte e = (byte)(context + 328);
	if (og_Main && colors_act_name && colors_base)
	{
		//theres DEFINITELY a better way to do this, but i couldnt find any atm. so this is a bit jank
		if (Sonic::CInputState::GetInstance()->GetPadState().IsTapped(Sonic::eKeyState_Y) && !switching)
		{
			switching = true;
			classic = !classic;
			if (classic)
			{
				colors_act_name->GetNode("act_name")->SetPatternIndex(0);
			}
			else
			{
				colors_act_name->GetNode("act_name")->SetPatternIndex(1);
			}
		}
		if (Sonic::CInputState::GetInstance()->GetPadState().IsTapped(Sonic::eKeyState_B) && !switching)
		{
			CSDCommon::PlayAnimation(*colors_base, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, false, true);
			CSDCommon::PlayAnimation(*colors_act_name, "Outro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		}
		printf("O");

	}
	return originalCHudGateMenuMainOutro(This, Edx);
}
HOOK(int, __fastcall, CHudGateMenuMainUsual, 0x1081170, int This, void* Edx)
{
	printf("U");	
	return originalCHudGateMenuMainUsual(This, Edx);
}
HOOK(int, __fastcall, CHudGateMenuMainIntroInfo, 0x1080110, hh::fnd::CStateMachineBase::CStateBase* This, void* Edx)
{
	////Run function first, otherwise scenes won't be gettable
	originalCHudGateMenuMainIntroInfo(This, Edx);

	int* contextBase = (int*)This->GetContextBase();
	projGate = *(Chao::CSD::RCPtr<Chao::CSD::CProject>*)((DWORD*)contextBase + 72);
	//Store original scenes
	og_Main = *(Chao::CSD::RCPtr<Chao::CSD::CScene>*)((uint32_t*)contextBase + 74);
	og_Main->SetHideFlag(false);
	og_Deco = *(Chao::CSD::RCPtr<Chao::CSD::CScene>*)((uint32_t*)contextBase + 76);
	og_Deco->GetNode("Null_deco")->SetHideFlag(true);

	colors_base = projGate->CreateScene("base");
	og_Main->GetNode("Null_bg_band")->SetHideFlag(true);
	colors_act_name = projGate->CreateScene("act_name");
	colors_score = projGate->CreateScene("score");
	colors_stageContainer = projGate->CreateScene("score_Clone");

	og_Main->GetNode("Null_mission_name")->SetPosition(-370,-50);
	og_Main->GetNode("misson_icon")->SetPosition(450,0);
	og_Main->GetNode("mission_name")->SetHideFlag(true);
	colors_score->GetNode("line_1")->SetPosition(0, 49/* * 2*/);
	colors_score->GetNode("line_2");
	colors_score->GetNode("line_3")->SetHideFlag(true);
	colors_score->GetNode("line_4")->SetHideFlag(true);
	colors_score->GetNode("line_5")->SetHideFlag(true);
	colors_score->GetNode("line_6")->SetHideFlag(true);
	colors_score->GetNode("act_0")->SetHideFlag(true);
	colors_score->GetNode("act_1")->SetHideFlag(true);
	colors_score->GetNode("act_2")->SetHideFlag(true);
	colors_score->GetNode("act_3")->SetHideFlag(true);
	colors_score->GetNode("act_4")->SetHideFlag(true);
	colors_score->GetNode("act_5")->SetHideFlag(true);
	colors_score->GetNode("act_6")->SetHideFlag(true);
	CSDCommon::PlayAnimation(*colors_score, "Usual_Anim", Chao::CSD::eMotionRepeatType_Loop, 1, 0);
	colors_score->SetPosition(1280 / 2, 720 / 2 - 100);
	colors_stageContainer->SetPosition(1280 / 2 - 8, 720 / 2 - 140);
	colors_score->SetScale(1, 0.65f);


	//classic??
	if ((int)contextBase[83] == 0)
	{
		classic = true;
		colors_act_name->GetNode("act_name")->SetPatternIndex(0);
	}
	else if ((int)contextBase[83] == 4) //modern??
	{
		classic = false;
		colors_act_name->GetNode("act_name")->SetPatternIndex(1);
	}
	//int e = contextBase;*//*
	//printf("\n%d", e);*//*
	//og_Deco->GetNode("style")->SetPatternIndex(e);	*/

	//SCU stuff
	CSDCommon::PlayAnimation(*colors_base, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*colors_act_name, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*colors_stageContainer, "Usual_Anim", Chao::CSD::eMotionRepeatType_Loop, 1, 0);
	colors_base->GetNode("word_completion")->SetHideFlag(true);
	colors_base->GetNode("bg")->SetPosition(1280 / 2, 720);
	og_Deco->GetNode("style")->SetHideFlag(true);
	auto vec = og_Deco->GetNode("stage")->GetPosition();
	og_Deco->GetNode("stage")->SetPosition(-48, -110);
	return 0;
}
#pragma region OldTest
HOOK(volatile signed __int32, *__fastcall, Test, 0x10804C0, hh::fnd::CStateMachineBase::CStateBase This, void* Edx)
{
	
	return originalTest(This, Edx);
}
HOOK(int, __fastcall, Gate_Update, 0x1081170, int a1) {
	return originalGate_Update(a1);
}
HOOK(char, *_stdcall, MoveToOtherStage, 0x107FBC0, int a1)
{
	printf("\n\nA1 %d", a1);
	printf("A1 %hkx\n", (byte)a1);
	switching = false;
	int param = a1;
	
		return originalMoveToOtherStage(a1);
}
void __fastcall Gate_CHudSonicStageRemoveCallback(Sonic::CGameObject* This, void*, Sonic::CGameDocument* pGameDocument)
{
	HudGate::KillScreen();

	if(colors_base)
	Chao::CSD::CProject::DestroyScene(projGate.Get(), colors_base);

	projGate = nullptr;
}
HOOK(void, __fastcall, Test5, 0x107F470, int* This)
{
	originalTest5(This);

}

#pragma endregion

HOOK(void*, __fastcall, Gate_UpdateApplication, 0xE7BED0, void* This, void* Edx, float elapsedTime, uint8_t a3)
{
	if (projGate)
	{
		if (colors_base->m_MotionFrame < 0)
			CSDCommon::PlayAnimation(*colors_base, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 0, 0, 1);
	}
	
	return originalGate_UpdateApplication(This, Edx, elapsedTime, a3);
}


HOOK(int, __stdcall, Gate_Test2, 0x107C960, int a1, int a2)
{
	FUNCTION_PTR(bool, __stdcall, sub_107ADC0, 0x107ADC0, int a1);
	/*Hedgehog::Base::CHolderBase f = new Hedgehog::Base::CHolderBase(Sonic::CApplicationDocument::ms_pInstance, false);
	auto g = */
	if (sub_107ADC0)
	{
		printf("yes");
	}
	else
	{
		printf("no");
	}
	printf("test");
	return originalGate_Test2(a1, a2);
}
HOOK(int, __fastcall, Gate_Test, 0x107DD70, hh::fnd::CStateMachineBase::CStateBase* This)
{
	int* context = (int*)This->GetContextBase();
	DWORD result = *(context + 328);
	switch (result)
	{
	case 2: {
		printf("2");
		break;
	}
	}
	printf("test");
	return originalGate_Test(This);
}
void HudGate::Install()
{
	INSTALL_HOOK(Gate_Test);
	INSTALL_HOOK(Gate_Test2);
	INSTALL_HOOK(CHudGateMenuMainIntroInfo);
	INSTALL_HOOK(CHudGateMenuMainOutro);
	INSTALL_HOOK(CHudGateMenuMainUsual);
	INSTALL_HOOK(MoveToOtherStage);
	INSTALL_HOOK(Gate_UpdateApplication);
}


//TODO: find a better way of loading the actual XNCP
