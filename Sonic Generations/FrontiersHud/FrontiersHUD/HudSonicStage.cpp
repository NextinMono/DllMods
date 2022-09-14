//CSD Object
boost::shared_ptr<Sonic::CGameObjectCSD> goGameplay, goFrontiers_2, goFrontiers_3;

//Projects
Chao::CSD::RCPtr<Chao::CSD::CProject> pGameplay, pFrontiers_2, pFrontiers_3;

//Scenes of projects (ideally keep them all separate from each project, unless they're used together)
Chao::CSD::RCPtr<Chao::CSD::CScene> gearCount, keyCount, boostCircle, lifeCount, boostCircleEmpty, boostTXT, skills, speed_ind, speedometer, ringMax, speedometerNumber, score;

float waitTimeBoost = 0;
bool hiddenBoost = true;

void HudSonicStage::CreateScreen(Sonic::CGameObject* pParentGameObject)
{
	if (pGameplay && !goGameplay)
		pParentGameObject->m_pMember->m_pGameDocument->AddGameObject(goGameplay = boost::make_shared<Sonic::CGameObjectCSD>(pGameplay, 0.5f, "HUD", false), "main", pParentGameObject);
	if (pFrontiers_2 && !goFrontiers_2)
		pParentGameObject->m_pMember->m_pGameDocument->AddGameObject(goFrontiers_2 = boost::make_shared<Sonic::CGameObjectCSD>(pFrontiers_2, 0.5f, "HUD", false), "main", pParentGameObject);
	if (pFrontiers_3 && !goFrontiers_3)
		pParentGameObject->m_pMember->m_pGameDocument->AddGameObject(goFrontiers_3 = boost::make_shared<Sonic::CGameObjectCSD>(pFrontiers_3, 0.5f, "HUD", false), "main", pParentGameObject);


}
void HudSonicStage::KillScreen()
{
	if (goGameplay)
	{
		goGameplay->SendMessage(goGameplay->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
		goGameplay = nullptr;
	}
	if (goFrontiers_2)
	{
		goFrontiers_2->SendMessage(goFrontiers_2->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
		goFrontiers_2 = nullptr;
	}
	if (goFrontiers_3)
	{
		goFrontiers_3->SendMessage(goFrontiers_3->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
		goFrontiers_3 = nullptr;
	}
}
void HudSonicStage::HudSonicStage::ToggleScreen(const bool visible, Sonic::CGameObject* pParentGameObject)
{
	if (visible)
	{
		HudSonicStage::CreateScreen(pParentGameObject);
	}
	else
		HudSonicStage::KillScreen();
}
void HudSonicStage_IntroFirstFrame(Chao::CSD::RCPtr<Chao::CSD::CScene> scene)
{
	scene->SetMotion("Intro_Anim");
	scene->SetMotionFrame(0);
	scene->m_MotionDisableFlag = 0;
	scene->m_MotionSpeed = 0;
	scene->Update(0);
}
void GetTime(Sonic::CGameDocument* pGameDocument, size_t* minutes, size_t* seconds, size_t* milliseconds)
{
	static uint32_t pAddr = 0xD61570;
	__asm
	{
		mov ecx, minutes
		mov edi, seconds
		mov esi, milliseconds
		mov eax, pGameDocument
		call[pAddr]
	}
}
//REMEMBER TO CALL NULLPTR HERE!!!!
void __fastcall CHudSonicStageRemoveCallback(Sonic::CGameObject* This, void*, Sonic::CGameDocument* pGameDocument)
{
	HudSonicStage::KillScreen();

	if (!Configuration::IsCyberspaceHUD)
	{
		Chao::CSD::CProject::DestroyScene(pGameplay.Get(), keyCount);
		Chao::CSD::CProject::DestroyScene(pGameplay.Get(), gearCount);
		Chao::CSD::CProject::DestroyScene(pGameplay.Get(), lifeCount);
		Chao::CSD::CProject::DestroyScene(pFrontiers_2.Get(), skills);
		Chao::CSD::CProject::DestroyScene(pFrontiers_3.Get(), speed_ind);
		Chao::CSD::CProject::DestroyScene(pFrontiers_3.Get(), speedometer);
		Chao::CSD::CProject::DestroyScene(pFrontiers_3.Get(), speedometerNumber);
		Chao::CSD::CProject::DestroyScene(pGameplay.Get(), ringMax);
		Chao::CSD::CProject::DestroyScene(pGameplay.Get(), score);
	}
	Chao::CSD::CProject::DestroyScene(pFrontiers_2.Get(), boostCircle);
	Chao::CSD::CProject::DestroyScene(pFrontiers_2.Get(), boostCircleEmpty);
	Chao::CSD::CProject::DestroyScene(pFrontiers_2.Get(), boostTXT);


	pGameplay, pFrontiers_2, pFrontiers_3 = nullptr;
	goGameplay, goFrontiers_2, goFrontiers_3 = nullptr;
	/*gearCount, keyCount, boostCircle, lifeCount, boostCircleEmpty, boostTXT, skills, speed_ind, speedometer, ringMax, speedometerNumber = nullptr;*/

}

HOOK(void, __fastcall, CHudSonicStageDelayProcessImp, 0x109A8D0, Sonic::CGameObject* This)
{
	originalCHudSonicStageDelayProcessImp(This);
	CHudSonicStageRemoveCallback(This, nullptr, nullptr);
	Sonic::CCsdDatabaseWrapper wrapper(This->m_pMember->m_pGameDocument->m_pMember->m_spDatabase.get());

	///Yea, I know, this code looks like spaghetti
	///but it's not my fault really, I wanted to have just 1 single merged XNCP
	///but shuriken decided to make everything get hidden if I saved.
	/// so instead now I have to use 3 xncps... yay.


	auto spCsdProject = wrapper.GetCsdProject("frontiers_interface_2");
	pFrontiers_2 = spCsdProject->m_rcProject;

	size_t& flags = ((size_t*)This)[151];

	spCsdProject = wrapper.GetCsdProject("ui_gameplay");
	pGameplay = spCsdProject->m_rcProject;
	spCsdProject = wrapper.GetCsdProject("frontiers_interface_3");
	pFrontiers_3 = spCsdProject->m_rcProject;


	//Rings
	//Gear
	//Key
	//Heart

	char gearsNum[16];
	char keyNum[16];
	char ringCap[16];
	sprintf(gearsNum, "%d", Configuration::GearsCount);
	sprintf(keyNum, "%d", Configuration::KeysCount);
	sprintf(ringCap, "/%d", Configuration::RingCapCount);
	//Ring cap
	ringMax = pGameplay->CreateScene("info_ring");
	ringMax->SetPosition(73, 13);
	ringMax->SetScale(0.8f, 0.8f);
	ringMax->GetNode("icon_ring")->SetHideFlag(true);
	ringMax->GetNode("num_ring")->SetText(ringCap);



	Configuration::Read();


	float offset = -70;
	float offsetLife = 0;
	float commonScale = 0.85f;
	Hedgehog::Math::CVector2* commonPos = new Hedgehog::Math::CVector2((1280 / 2) + 530, (720 / 2) + 270);
	if (Configuration::SpeedometerEnabled)
	{
		//Speedometer
		speedometer = pFrontiers_3->CreateScene("info_life");
		speedometer->SetPosition(commonPos->x() + 2, commonPos->y());
		speedometer->SetScale(commonScale, commonScale);
		speedometer->GetNode("icon_generic")->SetHideFlag(true);
		speed_ind = pFrontiers_3->CreateScene("info_life");
		speed_ind->SetPosition(commonPos->x(), commonPos->y());
		speed_ind->SetScale(commonScale - 0.1f, commonScale - 0.1f);
		speed_ind->GetNode("Cast_0235")->SetHideFlag(true);
		CSDCommon::PlayAnimation(*speedometer, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 100);
	}
	if (Configuration::SpeedometerNumberEnabled)
	{
		//Speedometer Number
		speedometerNumber = pFrontiers_3->CreateScene("info_ring");
		CSDCommon::FreezeMotion(*speedometerNumber);
		speedometerNumber->GetNode("icon_ring")->SetHideFlag(true);
		speedometerNumber->GetNode("num_ring")->SetHideFlag(true);
		//Move it next to the speedometer
		speedometerNumber->SetPosition(-40, 140);
		speedometerNumber->SetScale(0.9f, 0.9f);
	}
	if (Configuration::SkillsEnabled)
	{
		//Skills at the bottom left
		skills = pFrontiers_2->CreateScene("skills");
		skills->SetPosition(0, 90);
		skills->SetScale(0.9f, 0.9f);
	}
	if (Configuration::GearsKeysEnabled)
	{
		//Gears
		gearCount = pGameplay->CreateScene("info_life");
		gearCount->SetPosition(0, -70);
		gearCount->GetNode("num_sonic")->SetText(gearsNum);
		gearCount->GetNode("icon_generic")->SetPatternIndex(1);

		//Keys
		keyCount = pGameplay->CreateScene("info_life");
		keyCount->SetPosition(0, -35);
		keyCount->GetNode("num_sonic")->SetText(keyNum);
		keyCount->GetNode("icon_generic")->SetPatternIndex(2);
		offset = 0;
	}
	if (Configuration::LivesEnabled)
	{
		//Lives, it functions identically to the standard ui_gameplay lives, but I spawned it using code so I could move it without having to
		//find a function to get the original.
		lifeCount = pGameplay->CreateScene("info_life");
		lifeCount->SetPosition(0, offset);
		CSDCommon::FreezeMotion(*lifeCount);
	}




	//Disable boost bar if classic
	bool isClassic = !flags & 0x200;
	if (!isClassic) // Boost Gauge
	{
		boostCircle = pFrontiers_2->CreateScene("boostbar");
		boostCircleEmpty = pFrontiers_2->CreateScene("boostbar_sdw");
		boostCircle->SetScale(0.4f, 0.4f);
		boostCircleEmpty->SetScale(0.4f, 0.4f);
		boostTXT = pFrontiers_2->CreateScene("boostbartxt");
		boostTXT->SetScale(0.28f, 0.28f);
		CSDCommon::FreezeMotion(*boostTXT);
		CSDCommon::FreezeMotion(*boostCircleEmpty);
		CSDCommon::PlayAnimation(*boostCircle, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 100);
	}

	if (ScoreGenerationsAPI::IsAttached() && !ScoreGenerationsAPI::IsStageForbidden()) // Score
	{
		score = pGameplay->CreateScene("info_life");
		if (lifeCount)
		{
			score->SetPosition(0, offset + 35);
		}
		else
		{
			score->SetPosition(0, offset);
		}
		score->GetNode("icon_generic")->SetHideFlag(true);
	}
	flags &= ~(0x1 /*| 0x2 | 0x4 | 0x200 |*/ | 0x800); // Mask to prevent crash when game tries accessing the elements we disabled later on

	waitTimeBoost = 5;

	HudSonicStage::CreateScreen(This);
}

HOOK(void, __fastcall, CHudSonicStageUpdateParallel, 0x1098A50, Sonic::CGameObject* This, void* Edx, const hh::fnd::SUpdateInfo& in_rUpdateInfo)
{
	HudSonicStage::ToggleScreen(*(bool*)0x1A430D8, This); // ms_IsRenderGameMainHud
	originalCHudSonicStageUpdateParallel(This, Edx, in_rUpdateInfo);
	if (!goFrontiers_2 || !goFrontiers_3 || !goGameplay)
		return;


	char text[256];
	size_t rowIndex = 1;
	const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();

	if (playerContext->StateFlag(eStateFlag_Boost)
		|| (Sonic::CInputState::GetInstance()->GetPadState().IsTapped(Sonic::eKeyState_X) && playerContext->m_ChaosEnergy == 0))
	{
		waitTimeBoost = 0;
		hiddenBoost = false;
	}
	else
	{
		waitTimeBoost += in_rUpdateInfo.DeltaTime;
		hiddenBoost = waitTimeBoost >= 3;
	}

	if (lifeCount)
	{
		const size_t liveCountAddr = Common::GetMultiLevelAddress(0x1E66B34, { 0x4, 0x1B4, 0x7C, 0x9FDC });
		if (liveCountAddr)
		{
			sprintf(text, "%d", *(size_t*)liveCountAddr);
			lifeCount->GetNode("num_sonic")->SetText(text);
		}
	}
	if (score)
	{
		sprintf(text, Configuration::ScoreGenerationsFormat.c_str(), ScoreGenerationsAPI::GetScore());
		score->GetNode("num_sonic")->SetText(text);
	}
	if (boostCircle && playerContext)
	{
		if (!hiddenBoost)
		{
			//TYSM Skyth!
			auto& position = Sonic::Player::CPlayerSpeedContext::GetInstance()->m_spMatrixNode->m_Transform.m_Position;
			const auto camera = Sonic::CGameDocument::GetInstance()->GetWorld()->GetCamera();
			hh::math::CVector4 screenPosition = camera->m_MyCamera.m_View * hh::math::CVector4(position.x(), position.y(), position.z(), 1.0f);
			screenPosition = camera->m_MyCamera.m_Projection * screenPosition;
			screenPosition.head<2>() /= screenPosition.w();
			screenPosition.x() = ((screenPosition.x() * 0.5f + 0.5f) * 1280.0f) + 200;
			screenPosition.y() = (screenPosition.y() * -0.5f + 0.5f) * 720.0f - 100;
			boostCircle->SetPosition(screenPosition.x(), screenPosition.y());
			boostTXT->SetPosition(screenPosition.x(), screenPosition.y());
			boostCircleEmpty->SetPosition(screenPosition.x(), screenPosition.y());
			CSDCommon::PlayAnimation(*boostCircle, "gauge_up", Chao::CSD::eMotionRepeatType_PlayOnce, 1, playerContext->m_ChaosEnergy);

			float chaos2 = 100 - playerContext->m_ChaosEnergy;
		}

		boostCircle->SetHideFlag(hiddenBoost);
		boostCircleEmpty->SetHideFlag(hiddenBoost);
		boostTXT->SetHideFlag(hiddenBoost);
	}

	if (speed_ind && playerContext)
		CSDCommon::PlayAnimation(*speed_ind, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, playerContext->m_HorizontalVelocity.norm() / (playerContext->m_Is2DMode ? 45.0f : 90.0f) * 100.0f);

}

HOOK(void, __fastcall, MsgStartCommonButtonSign, 0x5289A0, void* thisDeclaration, void* edx, uint32_t a2)
{
	// Disable Y button prompt.
	if (*(uint32_t*)(a2 + 16) == 3)
		return;

	originalMsgStartCommonButtonSign(thisDeclaration, edx, a2);
}

HOOK(void, __fastcall, CHudSonicStageUpdate, 0x1098A50, void* thisDeclaration, void* edx, float* pUpdateInfo)
{
	// Force disable extended boost.
	*(uint32_t*)((uint32_t)*CONTEXT->ms_pInstance + 0x680) = 1;

	// Always clamp boost to 100.
	CONTEXT->m_ChaosEnergy = min(CONTEXT->m_ChaosEnergy, 100.0f);

	originalCHudSonicStageUpdate(thisDeclaration, edx, pUpdateInfo);
}

void HudSonicStage::Install()
{
	INSTALL_HOOK(CHudSonicStageDelayProcessImp);
	INSTALL_HOOK(CHudSonicStageUpdateParallel);
	INSTALL_HOOK(CHudSonicStageUpdate);
	INSTALL_HOOK(MsgStartCommonButtonSign);
	WRITE_MEMORY(0x109B1A4, uint8_t, 0xE9, 0xDC, 0x02, 0x00, 0x00); // Disable lives
	WRITE_MEMORY(0x16A467C, void*, CHudSonicStageRemoveCallback);
}