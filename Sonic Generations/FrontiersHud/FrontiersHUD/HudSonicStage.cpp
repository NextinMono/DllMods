//CSD Object
boost::shared_ptr<Sonic::CGameObjectCSD> goGameplay, goFrontiers_2;

//Projects
Chao::CSD::RCPtr<Chao::CSD::CProject> pGameplay, pFrontiers_2;

//Scenes of projects (ideally keep them all separate from each project, unless they're used together)
Chao::CSD::RCPtr<Chao::CSD::CScene> ringCount,ringOverlay, gearCount, keyCount, boostCircle, lifeCount, boostCircleEmpty, boostTXT, skills, speed_ind, speedometer, ringMax, speedometerNumber, score;
size_t prevRingCount = -1;
float waitTimeBoost = 0;
bool hiddenBoost = true;
bool ringSpun = false;
Hedgehog::Math::CVector2* offsetAspect;
Hedgehog::Math::CVector2* offsetRes;

void HudSonicStage::CreateScreen(Sonic::CGameObject* pParentGameObject)
{
	if (pGameplay && !goGameplay)
		pParentGameObject->m_pMember->m_pGameDocument->AddGameObject(goGameplay = boost::make_shared<Sonic::CGameObjectCSD>(pGameplay, 0.5f, "HUD", false), "main", pParentGameObject);
	if (pFrontiers_2 && !goFrontiers_2)
		pParentGameObject->m_pMember->m_pGameDocument->AddGameObject(goFrontiers_2 = boost::make_shared<Sonic::CGameObjectCSD>(pFrontiers_2, 0.5f, "HUD", false), "main", pParentGameObject);

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
		Chao::CSD::CProject::DestroyScene(pFrontiers_2.Get(), speed_ind);
		Chao::CSD::CProject::DestroyScene(pFrontiers_2.Get(), speedometer);
		Chao::CSD::CProject::DestroyScene(pFrontiers_2.Get(), speedometerNumber);
		Chao::CSD::CProject::DestroyScene(pFrontiers_2.Get(), ringOverlay);
		Chao::CSD::CProject::DestroyScene(pGameplay.Get(), ringMax);
		Chao::CSD::CProject::DestroyScene(pGameplay.Get(), ringCount);
		Chao::CSD::CProject::DestroyScene(pGameplay.Get(), score);
	}
	Chao::CSD::CProject::DestroyScene(pFrontiers_2.Get(), boostCircle);
	Chao::CSD::CProject::DestroyScene(pFrontiers_2.Get(), boostCircleEmpty);
	Chao::CSD::CProject::DestroyScene(pFrontiers_2.Get(), boostTXT);


	pGameplay, pFrontiers_2 = nullptr;
	goGameplay, goFrontiers_2 = nullptr;
	/*gearCount, keyCount, boostCircle, lifeCount, boostCircleEmpty, boostTXT, skills, speed_ind, speedometer, ringMax, speedometerNumber = nullptr;*/

}

HOOK(void, __fastcall, CHudSonicStageDelayProcessImp, 0x109A8D0, Sonic::CGameObject* This)
{
	originalCHudSonicStageDelayProcessImp(This);
	CHudSonicStageRemoveCallback(This, nullptr, nullptr);

	HudSonicStage::CalculateAspectOffsets();
	Sonic::CCsdDatabaseWrapper wrapper(This->m_pMember->m_pGameDocument->m_pMember->m_spDatabase.get());



	auto spCsdProject = wrapper.GetCsdProject("frontiers_interface_2");
	pFrontiers_2 = spCsdProject->m_rcProject;

	size_t& flags = ((size_t*)This)[151];

	spCsdProject = wrapper.GetCsdProject("ui_gameplay");
	pGameplay = spCsdProject->m_rcProject;


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
	ringMax->SetPosition(68, 15);
	ringMax->SetScale(0.7f, 0.7f);
	ringMax->GetNode("icon_ring")->SetHideFlag(true);
	ringMax->GetNode("num_ring")->SetText(ringCap);
	ringMax->GetNode("num_r_ring")->SetText(ringCap);


	ringCount = pGameplay->CreateScene("info_ring");
	ringCount->SetPosition(0, 0); //for some reason this makes it follow the aspect ratio...

	ringOverlay = pFrontiers_2->CreateScene("info_ring");
	ringOverlay->SetPosition(0, 0);
	CSDCommon::PlayAnimation(*ringOverlay, "get_Anim", Chao::CSD::eMotionRepeatType_Loop, 1, 100);
	


	Configuration::Read();


	float offset = -70;
	float offsetLife = 0;
	float commonScale = 0.85f;
	Hedgehog::Math::CVector2* commonPos = new Hedgehog::Math::CVector2(((LetterboxHelper::OriginalResolution->x() + offsetAspect->x())) - 70, ((LetterboxHelper::OriginalResolution->y() + offsetAspect->y())) - 60);
	if (Configuration::SpeedometerEnabled)
	{
		//Speedometer
		speedometer = pFrontiers_2->CreateScene("speedometer");
		speedometer->SetPosition(commonPos->x() + 2, commonPos->y());
		speedometer->SetScale(commonScale, commonScale);
		speedometer->GetNode("icon_generic")->SetHideFlag(true);

		speed_ind = pFrontiers_2->CreateScene("speed_handle");
		speed_ind->SetPosition(commonPos->x(), commonPos->y());
		speed_ind->SetScale(commonScale - 0.1f, commonScale - 0.1f);
		CSDCommon::PlayAnimation(*speedometer, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 100);
	}
	if (Configuration::SpeedometerNumberEnabled)
	{
		//Speedometer Number
		speedometerNumber = pFrontiers_2->CreateScene("speed_number");
		CSDCommon::FreezeMotion(*speedometerNumber);
		speedometerNumber->GetNode("icon_ring")->SetHideFlag(true);
		speedometerNumber->GetNode("num_ring")->SetHideFlag(true);
		//Move it next to the speedometer
		/*speedometerNumber->SetPosition(-40, 140);*/
		speedometerNumber->SetPosition(commonPos->x() - 65, commonPos->y() + 5);
		speedometerNumber->SetScale(0.9f, 0.9f);
	}
	if (Configuration::SkillsEnabled)
	{
		//Skills at the bottom left
		skills = pFrontiers_2->CreateScene("skill_cross");
		skills->SetPosition(95, ((LetterboxHelper::OriginalResolution->y() + offsetAspect->y())) - 75);
	}
	if (Configuration::GearsKeysEnabled)
	{
		//Gears
		gearCount = pGameplay->CreateScene("info_life");
		gearCount->SetPosition(0, -65);
		gearCount->GetNode("num_sonic")->SetText(gearsNum);
		gearCount->GetNode("icon_generic")->SetPatternIndex(1);

		//Keys
		keyCount = pGameplay->CreateScene("info_life");
		keyCount->SetPosition(0, -30);
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
		boostCircle->GetNode("circle")->SetRotation(90);
		boostCircleEmpty->SetScale(0.4f, 0.4f);
		boostTXT = pFrontiers_2->CreateScene("boostbar_txt");
		boostTXT->SetScale(0.3f, 0.3f);
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
	//~(0x1 | /*0x2/* | 0x4 | 0x200 | */ | 0x800); 
	flags &= ~(0x1 | 0x4 ); // Mask to prevent crash when game tries accessing the elements we disabled later on, disables Lives & Rings count

	waitTimeBoost = 5;

	HudSonicStage::CreateScreen(This);
}

HOOK(void, __fastcall, CHudSonicStageUpdateParallel, 0x1098A50, Sonic::CGameObject* This, void* Edx, const hh::fnd::SUpdateInfo& in_rUpdateInfo)
{
	HudSonicStage::ToggleScreen(*(bool*)0x1A430D8, This); // ms_IsRenderGameMainHud
	originalCHudSonicStageUpdateParallel(This, Edx, in_rUpdateInfo);
	if (!goFrontiers_2 || !goGameplay)
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
			screenPosition.x() = ((screenPosition.x() * 0.5f + 0.5f) * (LetterboxHelper::OriginalResolution->x() + offsetAspect->x())) + 150;
			screenPosition.y() = (screenPosition.y() * -0.5f + 0.5f) *(LetterboxHelper::OriginalResolution->y() + offsetAspect->y()) - 100;

			boostCircle->SetPosition(screenPosition.x(), screenPosition.y());
			boostTXT->SetPosition(screenPosition.x(), screenPosition.y());
			boostCircleEmpty->SetPosition(screenPosition.x(), screenPosition.y());
			CSDCommon::PlayAnimation(*boostCircle, "gauge_up", Chao::CSD::eMotionRepeatType_PlayOnce, 1, playerContext->m_ChaosEnergy);

		}

		boostCircle->SetHideFlag(hiddenBoost);
		boostCircleEmpty->SetHideFlag(hiddenBoost);
		boostTXT->SetHideFlag(hiddenBoost);
	}
	if (ringCount && playerContext)
	{
		sprintf(text, "%03d", playerContext->m_RingCount);
		ringCount->GetNode("num_ring")->SetText(text);

		if (prevRingCount < playerContext->m_RingCount)
		{

			CSDCommon::PlayAnimation(*ringCount, "zero_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
			CSDCommon::PlayAnimation(*ringCount, "get_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);

			CSDCommon::PlayAnimation(*ringMax, "zero_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
			CSDCommon::PlayAnimation(*ringMax, "get_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
			CSDCommon::PlayAnimation(*ringOverlay, "get_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		}
		if (prevRingCount != playerContext->m_RingCount && playerContext->m_RingCount < 1)
		{
			ringSpun = true;
			CSDCommon::PlayAnimation(*ringCount, "get_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
			CSDCommon::PlayAnimation(*ringCount, "zero_Anim", Chao::CSD::eMotionRepeatType_Loop, 1, 0);
			CSDCommon::PlayAnimation(*ringMax, "zero_Anim", Chao::CSD::eMotionRepeatType_Loop, 1, 0);
		}


		prevRingCount = playerContext->m_RingCount;
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
void HudSonicStage::CalculateAspectOffsets()
{
	if (*(size_t*)0x6F23C6 != 0x75D8C0D9) // Widescreen Support
	{
		const float aspect = (float)*(size_t*)0x1DFDDDC / (float)*(size_t*)0x1DFDDE0;

		offsetRes = new Hedgehog::Math::CVector2((float)*(size_t*)0x01804F8C, (float)*(size_t*)0x01804F90);

		if (aspect * 9.0f > 16.0f)
		{
			offsetAspect = new Hedgehog::Math::CVector2(720.0f * aspect - 1280.0f, 0.0f);
		}
		else
		{
			offsetAspect = new Hedgehog::Math::CVector2(0, 1280.0f / aspect - 720.0f);
		}
	}
	else
	{

		offsetAspect = new Hedgehog::Math::CVector2(0, 0);
	}
}

HOOK(int32_t*, __fastcall, CHudLoadingCStateOutroAdvance, 0x1092EE0, hh::fnd::CStateMachineBase::CStateBase* This)
{
	return originalCHudLoadingCStateOutroAdvance(This);
}
HOOK(int, __fastcall, MsgRestartStage, 0xE76810, Sonic::CGameObject* This, void* edx, int a2)
{
	hiddenBoost = false;
	if(boostCircle)
	boostCircle->SetHideFlag(true);
	if(boostCircleEmpty)
	boostCircleEmpty->SetHideFlag(true);
	if(boostTXT)
	boostTXT->SetHideFlag(true);
	return originalMsgRestartStage(This, edx, a2);
}
void HudSonicStage::Install()
{
	/*
	WRITE_MEMORY(0x016D2E4C, float, 480);
	WRITE_MEMORY(0x01703B38, float, 1120);*/
	INSTALL_HOOK(MsgRestartStage);
	INSTALL_HOOK(CHudLoadingCStateOutroAdvance);
	INSTALL_HOOK(CHudSonicStageDelayProcessImp);
	INSTALL_HOOK(CHudSonicStageUpdateParallel);
	INSTALL_HOOK(CHudSonicStageUpdate);
	INSTALL_HOOK(MsgStartCommonButtonSign);
	WRITE_MEMORY(0x109B1A4, uint8_t, 0xE9, 0xDC, 0x02, 0x00, 0x00); // Disable lives
	WRITE_MEMORY(0x109B5AD, uint8_t, 0x90, 0xE9); // Disable rings
	WRITE_MEMORY(0x16A467C, void*, CHudSonicStageRemoveCallback);
}