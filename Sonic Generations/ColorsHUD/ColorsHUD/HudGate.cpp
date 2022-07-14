
Chao::CSD::RCPtr<Chao::CSD::CProject> projGate;
boost::shared_ptr<Sonic::CGameObjectCSD> spGate;
Chao::CSD::RCPtr<Chao::CSD::CScene> colors_base, colors_act_name, og_Main, og_Deco;
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

HOOK(volatile signed __int32, *__fastcall, Test, 0x10804C0, hh::fnd::CStateMachineBase::CStateBase This, void* Edx)
{
	
	return originalTest(This, Edx);
}
HOOK(int, __fastcall, Gate_Update, 0x1081170, int a1) {
	return originalGate_Update(a1);
}
HOOK(char, *_stdcall, MoveToOtherStage, 0x107FBC0, int a1)
{
	return originalMoveToOtherStage(a1);
}
void __fastcall Gate_CHudSonicStageRemoveCallback(Sonic::CGameObject* This, void*, Sonic::CGameDocument* pGameDocument)
{
	HudGate::KillScreen();

	Chao::CSD::CProject::DestroyScene(projGate.Get(), colors_base);

	projGate = nullptr;
}
HOOK(void, __fastcall, Test5, 0x107F470, int* This)
{
	originalTest5(This);

}
HOOK(int, __fastcall, LoadGateInfo,0x1080110, uint32_t* This, void* Edx)
{	
	////Run function first, otherwise scenes won't be gettable
	originalLoadGateInfo(This, Edx);

	////Load original XNCP
	//auto& contextBase = This->GetContextBase();
	//projGate = *(Chao::CSD::RCPtr<Chao::CSD::CProject>*)((DWORD*)contextBase + 72);
	////Store original scenes
	//og_Main = *(Chao::CSD::RCPtr<Chao::CSD::CScene>*)((uint32_t*)contextBase + 74);
	//og_Main->SetHideFlag(true);
	//og_Deco = *(Chao::CSD::RCPtr<Chao::CSD::CScene>*)((uint32_t*)contextBase + 76);
	//og_Deco->GetNode("Null_deco")->SetHideFlag(true);
	//printf("%d", (size_t)contextBase + 83);
	//printf("\n%d", (int)contextBase + 83);
	//int e = contextBase
	//printf("\n%d", e);
	//og_Deco->GetNode("style")->SetPatternIndex(e);	

	////SCU stuff
	//colors_base = projGate->CreateScene("base");
	//colors_act_name = projGate->CreateScene("act_name");
	//CSDCommon::PlayAnimation(*colors_base, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);	
	//CSDCommon::PlayAnimation(*colors_act_name, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	//Hedgehog::Math::CVector2 pos = colors_act_name->GetNode("act_name")->GetPosition();
	//og_Deco->GetNode("style")->SetPosition(pos.x()- 80, pos.y() -180);
	return 0;
}
void HudGate::Install()
{
	INSTALL_HOOK(Test5);
	INSTALL_HOOK(LoadGateInfo);
	INSTALL_HOOK(MoveToOtherStage);
	INSTALL_HOOK(Gate_Update);
}


//TODO: find a better way of loading the actual XNCP
