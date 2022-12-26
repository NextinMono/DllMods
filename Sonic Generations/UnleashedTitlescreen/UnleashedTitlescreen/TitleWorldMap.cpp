#include <algorithm>
using namespace hh::math;

Chao::CSD::RCPtr<Chao::CSD::CProject> rcTitleScreenW;
Chao::CSD::RCPtr<Chao::CSD::CScene> infobg1, infoimg1, infoimg2, infoimg3, infoimg4, headerBGW, headerIMGW, footerBGW, footerIMGW;
Chao::CSD::RCPtr<Chao::CSD::CScene> cursorL, cursorT, cursorB, cursorR;
Chao::CSD::RCPtr<Chao::CSD::CScene> cts_name, cts_guide_window, cts_guide_ss, cts_guide_txt;
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
 CVector2 posT;
 CVector2 posB;
 CVector2 posL;
 CVector2 posR;
 bool disabledStick = true;
 bool disabledTarget = true;
 bool cursorSelected = false;

 bool playingPan = false;
 float timePan = 0;
 float camHeight = -20;

 void PlayCursorAnim(const char* name, bool reverse = false) 
 {
	 CSDCommon::PlayAnimation(*cursorL, name, Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0,0,false,reverse);
	 CSDCommon::PlayAnimation(*cursorR, name, Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, false, reverse);
	 CSDCommon::PlayAnimation(*cursorT, name, Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, false, reverse);
	 CSDCommon::PlayAnimation(*cursorB, name, Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, false, reverse);
 }
 void TitleWorldMap::PlayPanningAnim()
 {
	 disabledTarget = true;
	 disabledStick = true;
	 playingPan = true;
 }
 void TitleWorldMap::EnableInput()
 {
	 disabledStick = false;
 }
 void SetHideEverythingWM(bool hide)
 {
	 infobg1->SetHideFlag(hide);
	 infoimg1->SetHideFlag(hide);
	 infoimg2->SetHideFlag(hide);
	 infoimg3->SetHideFlag(hide);
	 infoimg4->SetHideFlag(hide);
	 headerBGW->SetHideFlag(hide);
	 headerIMGW->SetHideFlag(hide);
	 footerBGW->SetHideFlag(hide);
	 if (footerIMGW)
		 footerIMGW->SetHideFlag(hide);
	 cursorL->SetHideFlag(hide);
	 cursorT->SetHideFlag(hide);
	 cursorB->SetHideFlag(hide);
	 cursorR->SetHideFlag(hide);
	 cts_name->SetHideFlag(hide);
	 cts_guide_ss->SetHideFlag(hide);
	 cts_guide_window->SetHideFlag(hide);
	 cts_guide_txt->SetHideFlag(hide);
	for (size_t i = 0; i < 9; i++)
	{
		flag[i]->SetHideFlag(hide);
	}
 }
void TitleWorldMap::Start() 
{
	introPlayed = false;
	SetHideEverythingWM(false);	
	for (size_t i = 0; i < 9; i++)
	{
		CSDCommon::PlayAnimation(*flag[i], "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	}		
	CSDCommon::PlayAnimation(*infobg1, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*infoimg1, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*infoimg2, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*infoimg3, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*infoimg4, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*headerBGW, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 60);
	CSDCommon::PlayAnimation(*footerBGW, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 60);
	CSDCommon::PlayAnimation(*headerIMGW, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0); 
	PlayCursorAnim("Intro_Anim");
	CSDCommon::PlayAnimation(*cts_guide_ss, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*cts_guide_txt, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*cts_guide_window, "Intro_Anim_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
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
float lerpWithEaseInOut(float start, float end, float t) 
{
	Common::ClampFloat(t, 0, 1);
	t = (t < 0.5f) ? (2.0f * t * t) : (-1.0f + (4.0f - 2.0f * t) * t);  // Ease in/out
	return start + (end - start) * t;  // Interpolate and return result
}

void PlayPan(CustomCamera* camera, const Hedgehog::Universe::SUpdateInfo& updateInfo)
{
	if (timePan >= 2.5f)
	{
		playingPan = false;
		disabledTarget = false;
	}
	timePan += updateInfo.DeltaTime;
	camHeight = lerpWithEaseInOut(-20, 0, timePan / 2.5f);

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

	 CVector cameraTargetPosition = CVector(0, camHeight, 0);
	const CVector cameraVector = CVector(0, 0, cameraDistance);
	const CVector cameraPosition = cameraVector + cameraTargetPosition;

	// We can make this a parameter or something later.
	constexpr float rotationPitchRate = 2.0f;
	constexpr float rotationYawRate = 2.0f;

	if (!disabledStick) 
	{
		rotationPitch += -input.RightStickVertical * rotationPitchRate * updateInfo.DeltaTime;
		rotationYaw += input.RightStickHorizontal * rotationYawRate * updateInfo.DeltaTime;
	}

	// Gotta do this nonsense.
	constexpr float pitchMaxExtents = 70.0f * DEG2RAD; // Max rotation is 70 degrees in either direction,
													   // rather than 90, which would get us to the poles of the earth.
	// Now limit
	rotationPitch = fmax(-pitchMaxExtents, fmin(rotationPitch, pitchMaxExtents));
	// Cycle yaw so it doesn't go over 360, so we don't approach Very Large Numbers.
	rotationYaw = WrapAroundFloat(rotationYaw, 360.0 * DEG2RAD);

	const CQuaternion pitch = TitleWorldMap::QuaternionFromAngleAxis(rotationPitch, CVector(1, 0, 0));
	const CQuaternion yaw = TitleWorldMap::QuaternionFromAngleAxis(rotationYaw, CVector(0, 1, 0));

	const CQuaternion rotation = yaw * pitch;

	if(playingPan)
	PlayPan(camera, updateInfo);
	
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

	Chao::CSD::CProject::DestroyScene(rcTitleScreenW.Get(), cts_name);
	Chao::CSD::CProject::DestroyScene(rcTitleScreenW.Get(), cts_guide_ss);
	Chao::CSD::CProject::DestroyScene(rcTitleScreenW.Get(), cts_guide_window);
	Chao::CSD::CProject::DestroyScene(rcTitleScreenW.Get(), cts_guide_txt);
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
int lastFlagIn = 0;
bool IsInsideCursorRange(const CVector2& input, float visibility, int flagID)
{
	if (!cursorL || disabledStick)
		return false;
	bool result = false;
	float divider = 1.5f;
	// Compute the normal vectors of the four edges of the box
	CVector2 norm = input.normalized();
	CVector2 n1 = ((posR - posT) /divider).normalized();
	CVector2 n2 = ((posB - posR) / divider).normalized();
	CVector2 n3 = ((posL - posB) / divider).normalized();
	CVector2 n4 = ((posT - posL) / divider).normalized();

	// Check if the point p is on the same side of each edge as the other three corners
	bool inside = ((norm - posT).dot(n1) >= 0.8f) &&
		((norm - posR).dot(n2) >= 0.8f) &&
		((norm - posB).dot(n3) >= 0.8f) &&
		((norm - posL).dot(n4) >= 0.8f);
	if (inside)
	{
		if (visibility >= 80 && flagID != lastFlagIn)
		{
			printf("\nIN %d", flagID);			
			lastFlagIn = flagID;
			//for eventual night versions, add 9
			cts_guide_ss->GetNode("town_ss_img")->SetPatternIndex(flagID);
			cts_guide_txt->GetNode("text_size")->SetPatternIndex(flagID);

			cts_name->GetNode("img")->SetPatternIndex(flagID);
			result = true;
		}
	}
	else 
	{
		lastFlagIn = -1;
	}

	cts_name->SetHideFlag(result);
	cts_guide_ss->SetHideFlag(result);
	cts_guide_window->SetHideFlag(result);
	return result;
}
CVector2 WorldToUIPosition(const CVector& input)
{
	const auto camera = TitleWorldMap::Camera;
	if (!camera) return CVector2(0, 0);
	auto screenPosition = camera->m_MyCamera.m_View * CVector4(input.x(), input.y(), input.z(), 1.0f);
	screenPosition = camera->m_MyCamera.m_Projection * screenPosition;
	screenPosition.head<2>() /= screenPosition.w();
	screenPosition.x() = ((screenPosition.x() * 0.5f + 0.5f) * (LetterboxHelper::OriginalResolution->x() + offsetAspect->x()));
	screenPosition.y() = (screenPosition.y() * -0.5f + 0.5f) * (LetterboxHelper::OriginalResolution->y() + offsetAspect->y());
	return CVector2(screenPosition.x(), screenPosition.y());
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
	posL = CVector2(pos.x() - (sizeBox / 2), pos.y());
	posR = CVector2(pos.x() + sizeBox / 2, pos.y());
	posT = CVector2(pos.x(), pos.y() + sizeBox / 2);
	posB = CVector2(pos.x(), pos.y() - (sizeBox / 2));
	
	cursorL->SetPosition(posL.x() + LetterboxHelper::ScreenHalfPoint->x(), posL.y() + LetterboxHelper::ScreenHalfPoint->y());
	cursorR->SetPosition(posR.x() + LetterboxHelper::ScreenHalfPoint->x(), posR.y() + LetterboxHelper::ScreenHalfPoint->y());
	cursorT->SetPosition(posT.x() + LetterboxHelper::ScreenHalfPoint->x(), posT.y() + LetterboxHelper::ScreenHalfPoint->y());
	cursorB->SetPosition(posB.x() + LetterboxHelper::ScreenHalfPoint->x(), posB.y() + LetterboxHelper::ScreenHalfPoint->y());
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
	cts_guide_window = rcTitleScreenW->CreateScene("cts_guide_window");
	cts_guide_ss = rcTitleScreenW->CreateScene("cts_guide_ss");
	cts_guide_txt = rcTitleScreenW->CreateScene("cts_guide_txt");
	cts_name = rcTitleScreenW->CreateScene("cts_name");
	CSDCommon::PlayAnimation(*infobg1, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*infoimg1, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*infoimg2, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*infoimg3, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*infoimg4, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*headerBGW, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*footerBGW, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*footerBGW, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*headerIMGW, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*cts_name, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);

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
	PlayCursorAnim("Intro_Anim");
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

	SetHideEverythingWM(true);
	return originalTitleW_CMain(This, Edx, a2, a3, a4);
}

float editorMulti = 1;
HOOK(void*, __fastcall, TitleW_UpdateApplication, 0xE7BED0, Sonic::CGameObject* This, void* Edx, float elapsedTime, uint8_t a3)
{
	auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->m_CurrentPadStateIndex];
	float multiplier = 24;
	SetCursorPos(CVector(inputPtr->RightStickHorizontal * multiplier, -inputPtr->RightStickVertical * multiplier, 0));
	if (introPlayed)
	{
		/*if(inputPtr->IsTapped())*/
	}
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

		int amountSel = 0;
		if (TitleWorldMap::Camera)
		{
			for (size_t i = 0; i < 9; i++)
			{
				float g = fmax(0.0f, -(TitleWorldMap::Camera->m_MyCamera.m_Direction.dot((flagPositions->at(i) - CVector(0, 0, -2.34f)).normalized()))) * 100;
				auto uiPos = WorldToUIPosition(flagPositions->at(i));
				uiPos = (uiPos - CVector2(1280 / 2, 720 / 2).normalized() * 5.575f);
				if (flag[i]->m_MotionDisableFlag && !introPlayed)
				{
					introPlayed = true;
				}
				if (introPlayed)
					CSDCommon::PlayAnimation(*flag[i], "Fade_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 0, g, g);

				flag[i]->SetPosition(uiPos.x() , uiPos.y());
				bool inrange = IsInsideCursorRange(uiPos, g, i);
				amountSel += inrange;
			}
			if (amountSel > 0 && !cursorSelected)
			{
				cursorSelected = true;
				CSDCommon::PlayAnimation(*cts_name, "Intro_1_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
				PlayCursorAnim("Select_Anim");

				CSDCommon::PlayAnimation(*cts_guide_ss, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
				CSDCommon::PlayAnimation(*cts_guide_txt, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
				CSDCommon::PlayAnimation(*cts_guide_window, "Intro_Anim_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
			}
			else if(amountSel == 0 && cursorSelected) {
				cursorSelected = false;
				PlayCursorAnim("Select_Anim", true);
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