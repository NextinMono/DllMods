using namespace hh::math;

Chao::CSD::RCPtr<Chao::CSD::CProject> rcTitleScreenW;
Chao::CSD::RCPtr<Chao::CSD::CScene> infobg1, infoimg1, infoimg2, infoimg3, infoimg4, headerBGW, headerIMGW, footerBGW, footerIMGW;
Chao::CSD::RCPtr<Chao::CSD::CScene> cursorL, cursorT, cursorB, cursorR;
Chao::CSD::RCPtr<Chao::CSD::CScene> flag[9];
boost::shared_ptr<Sonic::CGameObjectCSD> spTitleScreenW;
std::vector < CVector> flagPositions[9];
static SharedPtrTypeless soundHandle;
bool playingPointerMove;
bool introPlayed = false;
CVector2* offsetAspect;
CVector2* offsetRes;
const CVector TitleWorldMap::TitleWorldMap::emblemPosition = CVector(0, 0, -2.34f);
const CustomCamera* TitleWorldMap::Camera;


void TitleWorldMap::Start() 
{
	introPlayed = false;
	infobg1->SetHideFlag(false);
	infoimg1->SetHideFlag(false);
	infoimg2->SetHideFlag(false);
	infoimg3->SetHideFlag(false);
	infoimg4->SetHideFlag(false);
	headerBGW->SetHideFlag(false);
	headerIMGW->SetHideFlag(false);
	footerBGW->SetHideFlag(false);
	if(footerIMGW)
	footerIMGW->SetHideFlag(false);
	cursorL->SetHideFlag(false);
	cursorT->SetHideFlag(false);
	cursorB->SetHideFlag(false);
	cursorR->SetHideFlag(false);
	for (size_t i = 0; i < 9; i++)
	{
		flag[i]->SetHideFlag(false);

		CSDCommon::PlayAnimation(*flag[i], "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	}
	for (auto e : flag) 
	{
		
	}
	CSDCommon::PlayAnimation(*infobg1, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*infoimg1, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*infoimg2, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*infoimg3, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*infoimg4, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*headerBGW, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 60);
	CSDCommon::PlayAnimation(*footerBGW, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 60);
	CSDCommon::PlayAnimation(*headerIMGW, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*cursorR, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*cursorL, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*cursorB, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*cursorT, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
}
#pragma region FromCeramic

class TransitionTitleCamera : public Sonic::CGameObject3D
{
public:
	int m_Unk00;
	boost::shared_ptr<CustomCamera> m_spCamera;
};

inline void __cdecl ApplyCameraStuff(TransitionTitleCamera* CameraImpl, CustomCamera* camera)
{
	static uint32_t func = 0x10FA1D0;
	__asm
	{
		mov edi, camera
		mov esi, CameraImpl
		call func
	}
}
CQuaternion TitleWorldMap::QuaternionFromAngleAxis(float angle, const CVector& axis)
{
	CQuaternion q;
	float m = sqrt(axis.x() * axis.x() + axis.y() * axis.y() + axis.z() * axis.z());
	float s = sinf(angle / 2) / m;
	q.x() = axis.x() * s;
	q.y() = axis.y() * s;
	q.z() = axis.z() * s;
	q.w() = cosf(angle / 2);
	return q;
}

static float rotationPitch = 0.0f;
static float rotationYaw = 0.0f;

float inline WrapAroundFloat(const float number, const float bounds)
{
	float result = number;

	if (number > bounds)
	{
		result = number - bounds;
	}

	if (number < 0)
	{
		result = number + bounds;
	}

	return result;
}

HOOK(void, __fastcall, _TitleCameraUpdate, 0x0058CDA0, TransitionTitleCamera* This, void*, const Hedgehog::Universe::SUpdateInfo& updateInfo)
{
	using namespace hh::math;
	auto* const camera = This->m_spCamera.get();
	TitleWorldMap::Camera = camera;

	auto input = Sonic::CInputState::GetInstance()->GetPadState();

	constexpr float cameraDistance = 20.0f;

	// HACK: Doing camera position stuff here instead of on a "Start" function, or the constructor, lol
	static bool initialized = false;
	if (!initialized)
	{
		initialized = true;
		camera->m_Position = CVector(0, 0, cameraDistance);
		camera->m_TargetPosition = CVector(0, 0, 0);
	}



	// Some stuff that'll help us in the future.

	const CVector cameraTargetPosition = CVector(0, 0, 0);
	const CVector cameraVector = CVector(0, 0, cameraDistance);
	const CVector cameraPosition = cameraVector + cameraTargetPosition;

	// We can make this a parameter or something later.
	constexpr float rotationPitchRate = 2.0f;
	constexpr float rotationYawRate = 2.0f;

	rotationPitch += -input.RightStickVertical * rotationPitchRate * updateInfo.DeltaTime;
	rotationYaw += input.RightStickHorizontal * rotationYawRate * updateInfo.DeltaTime;

	// Gotta do this nonsense.
	constexpr float pitchMaxExtents = 65.0f * DEG2RAD; // Max rotation is 65 degrees in either direction,
													   // rather than 90, which would get us to the poles of the earth.
	// Now limit
	rotationPitch = fmax(-pitchMaxExtents, fmin(rotationPitch, pitchMaxExtents));
	// Cycle yaw so it doesn't go over 360, so we don't approach Very Large Numbers.
	rotationYaw = WrapAroundFloat(rotationYaw, 360.0 * DEG2RAD);

	const CQuaternion pitch = TitleWorldMap::QuaternionFromAngleAxis(rotationPitch, CVector(1, 0, 0));
	const CQuaternion yaw = TitleWorldMap::QuaternionFromAngleAxis(rotationYaw, CVector(0, 1, 0));

	const CQuaternion rotation = yaw * pitch;


	// Now we rotate everything.
	camera->m_Position = rotation * (cameraPosition - TitleWorldMap::emblemPosition) + TitleWorldMap::emblemPosition;
	camera->m_TargetPosition = rotation * (cameraTargetPosition - TitleWorldMap::emblemPosition) + TitleWorldMap::emblemPosition;
	// This happens after the fact for some reason. Wonder why...

	const float FOV = 0.44906584f;
	camera->m_FieldOfView = FOV;
	camera->m_FieldOfViewTarget = FOV;

	ApplyCameraStuff(This, camera);
	camera->UpdateParallel(updateInfo);


}
#pragma endregion


void CalculateAspectOffsets()
{
	if (*(size_t*)0x6F23C6 != 0x75D8C0D9) // Widescreen Support
	{
		const float aspect = (float)*(size_t*)0x1DFDDDC / (float)*(size_t*)0x1DFDDE0;

		offsetRes = new CVector2((float)*(size_t*)0x01804F8C, (float)*(size_t*)0x01804F90);

		if (aspect * 9.0f > 16.0f)
		{
			offsetAspect = new CVector2(720.0f * aspect - 1280.0f, 0.0f);
		}
		else
		{
			offsetAspect = new CVector2(0, 1280.0f / aspect - 720.0f);
		}
	}
	else
	{

		offsetAspect = new CVector2(0, 0);
	}
}
void __fastcall CTitleWRemoveCallback(Sonic::CGameObject* This, void*, Sonic::CGameDocument* pGameDocument)
{
	TitleWorldMap::KillScreen();
	Chao::CSD::CProject::DestroyScene(rcTitleScreenW.Get(), infobg1);
	Chao::CSD::CProject::DestroyScene(rcTitleScreenW.Get(), infoimg2);
	Chao::CSD::CProject::DestroyScene(rcTitleScreenW.Get(), infoimg3);
	Chao::CSD::CProject::DestroyScene(rcTitleScreenW.Get(), infoimg4);
	Chao::CSD::CProject::DestroyScene(rcTitleScreenW.Get(), headerBGW);
	Chao::CSD::CProject::DestroyScene(rcTitleScreenW.Get(), headerIMGW);
	Chao::CSD::CProject::DestroyScene(rcTitleScreenW.Get(), footerBGW);
	Chao::CSD::CProject::DestroyScene(rcTitleScreenW.Get(), footerIMGW);

	Chao::CSD::CProject::DestroyScene(rcTitleScreenW.Get(), cursorL);
	Chao::CSD::CProject::DestroyScene(rcTitleScreenW.Get(), cursorT);
	Chao::CSD::CProject::DestroyScene(rcTitleScreenW.Get(), cursorB);
	Chao::CSD::CProject::DestroyScene(rcTitleScreenW.Get(), cursorR);
	rcTitleScreenW = nullptr;

}
void TitleWorldMap::CreateScreen(Sonic::CGameObject* pParentGameObject)
{
	if (rcTitleScreenW && !spTitleScreenW)
	{
		pParentGameObject->m_pMember->m_pGameDocument->AddGameObject(spTitleScreenW = boost::make_shared<Sonic::CGameObjectCSD>(rcTitleScreenW, 0.5f, "HUD", false), "main", pParentGameObject);

		pParentGameObject->SendMessage(pParentGameObject->m_ActorID, boost::make_shared<Sonic::Message::MsgSetGlobalLightDirection>(CQuaternion(1, 1, 1, 1)));
	}
}

void TitleWorldMap::KillScreen()
{
	if (spTitleScreenW)
	{
		spTitleScreenW->SendMessage(spTitleScreenW->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
		spTitleScreenW = nullptr;
	}
}

void TitleWorldMap::ToggleScreen(const bool visible, Sonic::CGameObject* pParentGameObject)
{
	if (visible)
		CreateScreen(pParentGameObject);
	else
		KillScreen();
}

void TitleWorldMap::IntroAnim(Chao::CSD::RCPtr<Chao::CSD::CScene> scene)
{
	scene->SetMotion("Intro_Anim");
	scene->SetMotionFrame(0.0f);
	scene->m_MotionSpeed = 1;
	scene->m_MotionRepeatType = Chao::CSD::eMotionRepeatType_PlayOnce;
	scene->Update(0.0f);
}
bool IsInsideCursorRange(const CVector& input)
{
	if (!cursorL)
		return false;
	bool result = false;
	auto posL = cursorL->GetNode("center_position")->GetPosition();
	auto posT = cursorT->GetNode("center_position")->GetPosition();
	auto posR = cursorR->GetNode("center_position")->GetPosition();
	auto posB = cursorB->GetNode("center_position")->GetPosition();
	if (input.x() >= posL.x() && input.x() <= posR.x() && input.y() >= posB.x() && input.y() <= posT.x())
	{
		MessageBoxA(NULL, "f", "f", 0);
		result = true;
	}

	return result;
}
CVector4 WorldToUIPosition(const CVector& input)
{
	const auto camera = TitleWorldMap::Camera;
	if (!camera) return CVector4(0, 0, 0, 0);
	auto screenPosition = camera->m_MyCamera.m_View * CVector4(input.x(), input.y(), input.z(), 1.0f);
	screenPosition = camera->m_MyCamera.m_Projection * screenPosition;
	screenPosition.head<2>() /= screenPosition.w();
	screenPosition.x() = ((screenPosition.x() * 0.5f + 0.5f) * (LetterboxHelper::OriginalResolution->x() + offsetAspect->x()));
	screenPosition.y() = (screenPosition.y() * -0.5f + 0.5f) * (LetterboxHelper::OriginalResolution->y() + offsetAspect->y());
	return screenPosition;
}

//float GetFlagVisibility(const CVector& input) {
//	 input.dot()
//}
void SetCursorPos(const CVector& pos)
{
	if (!cursorL)
		return;
	if (((pos.x() + pos.y()) != 0) && !playingPointerMove) {
		Common::PlaySoundStatic(soundHandle, 800002);
		playingPointerMove = true;
	}
	else if (((pos.x() + pos.y()) == 0) && playingPointerMove)
	{
		playingPointerMove = false;
		soundHandle.reset();
	}
	float sizeBox = 100;

	cursorL->SetPosition(pos.x() - (sizeBox / 2), pos.y());
	cursorR->SetPosition(pos.x() + sizeBox / 2, pos.y());
	cursorT->SetPosition(pos.x(), pos.y() + sizeBox / 2);
	cursorB->SetPosition(pos.x(), pos.y() - (sizeBox / 2));
	cursorR->GetNode("arrow_position_9")->SetRotation(180);
	cursorT->GetNode("arrow_position_9")->SetRotation(90);
	cursorB->GetNode("arrow_position_9")->SetRotation(-90);
}
HOOK(int, __fastcall, TitleW_CMain, 0x0056FBE0, Sonic::CGameObject* This, void* Edx, int a2, int a3, void** a4)
{
	CTitleWRemoveCallback(This, nullptr, nullptr);
	CalculateAspectOffsets();
	Sonic::CCsdDatabaseWrapper wrapper(This->m_pMember->m_pGameDocument->m_pMember->m_spDatabase.get());
	auto spCsdProject = wrapper.GetCsdProject("ui_worldmap");
	rcTitleScreenW = spCsdProject->m_rcProject;
	//infobg1, infoimg2, infoimg3, infoimg4, headerBGW, headerIMGW, footerBGW, footerIMGW;
	infobg1 = rcTitleScreenW->CreateScene("info_bg_1");
	infoimg1 = rcTitleScreenW->CreateScene("info_img_1");
	infoimg2 = rcTitleScreenW->CreateScene("info_img_2");
	infoimg3 = rcTitleScreenW->CreateScene("info_img_3");
	infoimg4 = rcTitleScreenW->CreateScene("info_img_4");

	headerBGW = rcTitleScreenW->CreateScene("worldmap_header_bg");
	footerBGW = rcTitleScreenW->CreateScene("worldmap_footer_bg");
	footerIMGW = rcTitleScreenW->CreateScene("worldmap_footer_img");
	headerIMGW = rcTitleScreenW->CreateScene("worldmap_header_img");
	CSDCommon::PlayAnimation(*infobg1, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*infoimg1, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*infoimg2, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*infoimg3, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*infoimg4, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*headerBGW, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*footerBGW, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*footerBGW, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*headerIMGW, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);

	for (size_t i = 0; i < 9; i++)
	{
		flag[i] = rcTitleScreenW->CreateScene("cts_parts_flag");
		flag[i]->GetNode("img")->SetPatternIndex(i);
		CSDCommon::PlayAnimation(*flag[i], "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	}
	//By default the cursor in the worldmap is set to the left anchor 
	cursorL = rcTitleScreenW->CreateScene("cts_cursor");
	cursorT = rcTitleScreenW->CreateScene("cts_cursor");
	cursorB = rcTitleScreenW->CreateScene("cts_cursor");
	cursorR = rcTitleScreenW->CreateScene("cts_cursor");
	CSDCommon::PlayAnimation(*cursorR, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*cursorL, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*cursorB, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*cursorT, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	float earthRadius = 5.575f;
	//flagPositions->push_back((CVector(0.31f, 0.36f, 2.28f) - TitleWorldMap::emblemPosition).normalized() * earthRadius);
	//flagPositions->push_back((CVector(2.310000f, 2.360000f, 1.111371f) - TitleWorldMap::emblemPosition).normalized() * earthRadius);
	//flagPositions->push_back((CVector(2.810000f, -0.140000f, -6.649425f) - TitleWorldMap::emblemPosition).normalized() * earthRadius);
	//flagPositions->push_back((CVector(2.810000f, -1.890000, 1.742745) - TitleWorldMap::emblemPosition).normalized() * earthRadius);
	//flagPositions->push_back((CVector(-0.190000, 4.610000, -3.543527) - TitleWorldMap::emblemPosition).normalized() * earthRadius);
	//flagPositions->push_back((CVector(-5.190000, 0.110000, -3.363136) - TitleWorldMap::emblemPosition).normalized() * earthRadius);
	//flagPositions->push_back((CVector(0.060000, -2.639999, -6.829812) - TitleWorldMap::emblemPosition).normalized() * earthRadius);
	//flagPositions->push_back((CVector(-4.440000, -2.390000, -0.798426) - TitleWorldMap::emblemPosition).normalized() * earthRadius);
	//flagPositions->push_back((CVector(-3.690000, 3.360000, -1.163138) - TitleWorldMap::emblemPosition).normalized() * earthRadius);
	//flagPositions->push_back((CVector(0.31f, 0.36f, 2.28f) - TitleWorldMap::emblemPosition).normalized() * earthRadius);
	flagPositions->push_back(CVector(0.31f, 0.36f, 2.28f));
	flagPositions->push_back(CVector(2.310000f, 2.360000f, 1.111371f));
	flagPositions->push_back(CVector(2.810000f, -0.140000f, -6.649425f));
	flagPositions->push_back(CVector(2.810000f, -1.890000, 1.742745));
	flagPositions->push_back(CVector(-0.190000, 4.610000, -3.543527));
	flagPositions->push_back(CVector(-5.190000, 0.110000, -3.363136));
	flagPositions->push_back(CVector(0.060000, -2.639999, -6.829812));
	flagPositions->push_back(CVector(-4.440000, -2.390000, -0.798426));
	flagPositions->push_back(CVector(-3.690000, 3.360000, -1.163138)); //-4.440000 y: -2.390000 z : -0.798426
	flagPositions->push_back(CVector(0.31f, 0.36f, 2.28f));



	TitleWorldMap::CreateScreen(This);
	for (auto e : flag)
	{
		e->SetHideFlag(true);
	}

	infobg1->SetHideFlag(true);
	infoimg1->SetHideFlag(true);
	infoimg2->SetHideFlag(true);
	infoimg3->SetHideFlag(true);
	infoimg4->SetHideFlag(true);
	headerBGW->SetHideFlag(true);
	headerIMGW->SetHideFlag(true);
	footerBGW->SetHideFlag(true);
	if(footerIMGW)
	footerIMGW->SetHideFlag(true);
	cursorL->SetHideFlag(true);
	cursorT->SetHideFlag(true);
	cursorB->SetHideFlag(true);
	cursorR->SetHideFlag(true);
	return originalTitleW_CMain(This, Edx, a2, a3, a4);
}

float editorMulti = 1;
HOOK(void*, __fastcall, TitleW_UpdateApplication, 0xE7BED0, Sonic::CGameObject* This, void* Edx, float elapsedTime, uint8_t a3)
{
	auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->m_CurrentPadStateIndex];
	float multiplier = 24;
	SetCursorPos(CVector(inputPtr->RightStickHorizontal * multiplier, -inputPtr->RightStickVertical * multiplier, 0));
	if (flag[0])
	{
		/*if (static_cast<uint8_t>(GetAsyncKeyState(VK_CONTROL))) {
			editorMulti -= 0.25f;
		}
		if (static_cast<uint8_t>(GetAsyncKeyState(VK_SHIFT))) {
			editorMulti += 0.25f;
		}
		if (editorMulti < 0) editorMulti = 0;
		flagPositions->at(7).x() += static_cast<uint8_t>(GetAsyncKeyState(VK_RIGHT)) * editorMulti;
		flagPositions->at(7).x() -= static_cast<uint8_t>(GetAsyncKeyState(VK_LEFT)) * editorMulti;

		flagPositions->at(7).y() += static_cast<uint8_t>(GetAsyncKeyState(VK_UP)) * editorMulti;
		flagPositions->at(7).y() -= static_cast<uint8_t>(GetAsyncKeyState(VK_DOWN)) * editorMulti;*/

		/*flagPositions->at(7).z() -= inputPtr->RightTrigger;
		flagPositions->at(7).z() += inputPtr->LeftTrigger;*/

		/*if (inputPtr->IsDown(Sonic::eKeyState_LeftBumper))
		{
			MessageBoxA(NULL, "Check Console for Output", "", 0);
			printf("\nx: %f", flagPositions->at(7).x());
			printf(" y: %f", flagPositions->at(7).y());
			printf(" z: %f", flagPositions->at(7).z());
		}*/

		if (TitleWorldMap::Camera)
		{
			for (size_t i = 0; i < 9; i++)
			{
				float g = fmax(0.0f, -(TitleWorldMap::Camera->m_MyCamera.m_Direction.dot((flagPositions->at(i) - CVector(0, 0, -2.34f)).normalized()))) * 100;
				auto uiPos = WorldToUIPosition(flagPositions->at(i));
				uiPos = (uiPos - CVector4(1280 / 2, 720 / 2,0,1).normalized() * 5.575f);
				if (flag[i]->m_MotionDisableFlag && !introPlayed)
				{
					introPlayed = true;
				}
				if (introPlayed)
					CSDCommon::PlayAnimation(*flag[i], "Fade_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 0, g, g);

				flag[i]->SetPosition(uiPos.x() , uiPos.y());

			}
		}
	}
	return originalTitleW_UpdateApplication(This, Edx, elapsedTime, a3);
}



void TitleWorldMap::Install()
{
	INSTALL_HOOK(_TitleCameraUpdate);
	INSTALL_HOOK(TitleW_CMain);
	INSTALL_HOOK(TitleW_UpdateApplication);
	WRITE_MEMORY(0x016E11F4, void*, CTitleWRemoveCallback);
}