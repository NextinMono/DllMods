Chao::CSD::RCPtr<Chao::CSD::CProject> rcBtnGuideColors;
Chao::CSD::RCPtr<Chao::CSD::CScene> trick_text, onebtn;
boost::shared_ptr<Sonic::CGameObjectCSD> spBtnGuideColors;

bool moved = false;
bool tricksStarted = false;
bool introAnimPlayed = false;
int textChoice = -1;
float timeBetweenAnim = 0;


void HudGpTricks::CreateScreen(Sonic::CGameObject* pParentGameObject)
{
	if (rcBtnGuideColors && !spBtnGuideColors)
		pParentGameObject->m_pMember->m_pGameDocument->AddGameObject(spBtnGuideColors = boost::make_shared<Sonic::CGameObjectCSD>(rcBtnGuideColors, 0.5f, "HUD", false), "main", pParentGameObject);
}
void HudGpTricks::KillScreen()
{
	if (spBtnGuideColors)
	{
		spBtnGuideColors->SendMessage(spBtnGuideColors->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
		spBtnGuideColors = nullptr;
	}
}

void HudGpTricks::ToggleScreen(const bool visible, Sonic::CGameObject* pParentGameObject)
{
	if (visible)
	{
		HudGpTricks::CreateScreen(pParentGameObject);
	}
	else
		HudGpTricks::KillScreen();
}
void __fastcall HGT_CHudSonicStageRemoveCallback(Sonic::CGameObject* This, void*, Sonic::CGameDocument* pGameDocument)
{
	HudGpTricks::KillScreen();

	Chao::CSD::CProject::DestroyScene(rcBtnGuideColors.Get(), trick_text);
	Chao::CSD::CProject::DestroyScene(rcBtnGuideColors.Get(), onebtn);
	rcBtnGuideColors = nullptr;
	tricksStarted = false;
	introAnimPlayed = false;
	textChoice = -1;

}

HOOK(void, __fastcall, HGT_CHudSonicStageDelayProcessImp, 0x109A8D0, Sonic::CGameObject* This)
{
	originalHGT_CHudSonicStageDelayProcessImp(This);

	HGT_CHudSonicStageRemoveCallback(This, nullptr, nullptr);
	Sonic::CCsdDatabaseWrapper wrapper(This->m_pMember->m_pGameDocument->m_pMember->m_spDatabase.get());

	auto spCsdProject = wrapper.GetCsdProject("ui_gameplay_btn_guide");
	rcBtnGuideColors = spCsdProject->m_rcProject;
	trick_text = rcBtnGuideColors->CreateScene("trickjump");
	onebtn = rcBtnGuideColors->CreateScene("1btn");

	trick_text->GetNode("text")->SetPatternIndex(0);
	trick_text->GetNode("text_0001")->SetPatternIndex(0);
	CSDCommon::PlayAnimation(*trick_text, "Outro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	trick_text->SetHideFlag(true);

	onebtn->GetNode("icon_btn")->SetPatternIndex(35);
	CSDCommon::PlayAnimation(*onebtn, "Outro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	onebtn->SetHideFlag(true);

	HudGpTricks::CreateScreen(This);
}
HOOK(void, __fastcall, StartTricks, 0x52F030, Sonic::CGameObject* This, void* edx, int a2)
{
	originalStartTricks(This, edx, a2);
	textChoice = -1;
	tricksStarted = true;
	trick_text->SetHideFlag(false);
	onebtn->SetHideFlag(false);
	CSDCommon::PlayAnimation(*onebtn, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
}

HOOK(void, __fastcall, AdvanceTricks, 0x52F390, Sonic::CGameObject* This, void* edx, int a2)
{
	textChoice++;
	if (textChoice > 3)
		textChoice = 3;

	trick_text->GetNode("text")->SetPatternIndex(textChoice);
	trick_text->GetNode("text_0001")->SetPatternIndex(textChoice);
	trick_text->m_MotionDisableFlag = 0;
	trick_text->SetHideFlag(false);
	CSDCommon::PlayAnimation(*trick_text, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	introAnimPlayed = true;
	return originalAdvanceTricks(This, edx, a2);
}
HOOK(void, __fastcall, HGT_CHudSonicStageUpdateParallel, 0x1098A50, Sonic::CGameObject* This, void* Edx, const hh::fnd::SUpdateInfo& in_rUpdateInfo)
{	
	if (introAnimPlayed)
	{
		if (trick_text->m_MotionDisableFlag == 1)
		{
			CSDCommon::PlayAnimation(*trick_text, "Outro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);

			introAnimPlayed = false;
		}
		if (textChoice <= 0 && tricksStarted && onebtn->m_MotionDisableFlag == 1)
		{
			CSDCommon::PlayAnimation(*onebtn, "Usual_Anim", Chao::CSD::eMotionRepeatType_Loop, 1, 0);
		}
	}
}
HOOK(void, __fastcall, FinalTrick, 0x52F8F0, Sonic::CGameObject* This, void* Edx, int a2)
{
	const byte mode = *(byte*)(a2 + 16);

	tricksStarted = false;
	if (mode != 0)
	{
		trick_text->GetNode("text")->SetPatternIndex(4);
		trick_text->GetNode("text_0001")->SetPatternIndex(4);
		CSDCommon::PlayAnimation(*trick_text, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		introAnimPlayed = true;
	}
	CSDCommon::PlayAnimation(*onebtn, "Outro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	textChoice = -1;
	return originalFinalTrick(This, Edx, a2);
}
HOOK(void, __fastcall, Test,  0x11B3790, Sonic::CGameObject* This, void* Edx, int a2, int a3, int* a4)
{
	return originalTest(This, Edx, a2, a3, a4);
}
void HudGpTricks::Install()
{	
	INSTALL_HOOK(Test);
	INSTALL_HOOK(StartTricks);
	INSTALL_HOOK(FinalTrick);
	INSTALL_HOOK(AdvanceTricks);
	INSTALL_HOOK(HGT_CHudSonicStageUpdateParallel);
	INSTALL_HOOK(HGT_CHudSonicStageDelayProcessImp);

}