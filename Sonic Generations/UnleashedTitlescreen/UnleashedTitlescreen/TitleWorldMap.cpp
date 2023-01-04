#include <algorithm>	
// I recommend committing these to Pch.h whenever you have this header,	
// it's much more useful for debugging than print spam.	
//#define DEBUG_DRAW_TEXT_DLL_IMPORT	
//#include "DebugDrawText.h"	

using namespace hh::math;


constexpr double RAD2DEG = 57.29578018188477;
constexpr double DEG2RAD = 0.01745329238474369;

Chao::CSD::RCPtr<Chao::CSD::CProject> rcTitleScreenW;
Chao::CSD::RCPtr<Chao::CSD::CScene> infobg1, infoimg1, infoimg2, infoimg3, infoimg4, headerBGW, headerIMGW, footerBGW, footerIMGW;
Chao::CSD::RCPtr<Chao::CSD::CScene> cursorL, cursorT, cursorB, cursorR;
Chao::CSD::RCPtr<Chao::CSD::CScene> cts_name, cts_guide_window, cts_guide_ss, cts_guide_txt, cts_guide_4_misson, cts_guide_5_medal;
Chao::CSD::RCPtr<Chao::CSD::CScene> cts_stage_window, cts_stage_select, cts_name_2, stageSelectFlag, cts_stage_info_window, cts_guide_1_hiscore, cts_guide_2_besttime, cts_guide_3_rank, cts_stage_4_misson, cts_stage_5_medal;
Chao::CSD::RCPtr<Chao::CSD::CScene> flag[9];
Chao::CSD::RCPtr<Chao::CSD::CScene> deco_text[6];
Chao::CSD::RCPtr<Chao::CSD::CScene> textWorldDesc, textStageDesc;

boost::shared_ptr<Sonic::CGameObjectCSD> spTitleScreenW;
std::vector<CVector> flagPositions;
static SharedPtrTypeless cursorMoveHandle, cursorSelectHandle, stageSelectHandle;
const CVector TitleWorldMap::TitleWorldMap::emblemPosition = CVector(0, 0, -2.34f);
const CustomCamera* TitleWorldMap::Camera;
CVector2 posCursorCenter;
CVector2* offsetAspect;
CVector2* offsetRes;


static float rotationPitch = 0.0f;
static float rotationYaw = 0.0f;
int stageSelectedWindow = 0;
int stageSelectedWindowMax = 6;

bool stageWindowOpen;
bool playingPointerMove;
bool introPlayed = false;
bool disabledStick = true;
bool disabledTarget = true;
bool cursorSelected = false;
bool playingPan = false;
float timePan, timeStageSelectDelay = 0;
float camHeight = -20;
float lastAmountSel = 0;
float editorMulti = 1;
int currentFlagSelected, lastValidFlagSelected, lastFlagIn = 0;
hh::fnd::CStateMachineBase::CStateBase* testState;

//Sonic::CGameObject *__thiscall sub_A51F90(Sonic::CGameObject *this, int a2, float a3)
//FUNCTION_PTR(byte, __fastcall, sub_A51F90, 0xA51F40, int* This, int a2, float* a3, void* Edx);
//FUNCTION_PTR(int*, __fastcall, sub_A515F0, 0xA515F0);
//FUNCTION_PTR(int, __stdcall, sub_D699A0, 0xD699A0, Sonic::CGameObject* a1);
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
void PlayCursorAnim(const char* name, bool reverse = false)
{
	CSDCommon::PlayAnimation(*cursorL, name, Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, false, reverse);
	CSDCommon::PlayAnimation(*cursorR, name, Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, false, reverse);
	CSDCommon::PlayAnimation(*cursorT, name, Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, false, reverse);
	CSDCommon::PlayAnimation(*cursorB, name, Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, false, reverse);
}
void CheckCursorAnimStatus()
{
	if (cts_guide_window)
	{
		if (cts_guide_window->m_MotionFrame >= 85 && cts_guide_window->m_MotionFrame <= 90)
		{
			cts_guide_4_misson->SetHideFlag(false);
			cts_guide_5_medal->SetHideFlag(false);
		}
	}
	/*if (cursorSelected)
		return;
	if (!cursorL->m_MotionDisableFlag && cursorL->m_MotionFrame <= 0)
		CSDCommon::FreezeMotion(*cursorL);

	if (!cursorR->m_MotionDisableFlag && cursorR->m_MotionFrame <= 0 )
		CSDCommon::FreezeMotion(*cursorR);

	if (!cursorT->m_MotionDisableFlag && cursorT->m_MotionFrame <= 0 )
		CSDCommon::FreezeMotion(*cursorT);

	if (!cursorB->m_MotionDisableFlag && cursorB->m_MotionFrame <= 0)
		CSDCommon::FreezeMotion(*cursorB);*/
}
void SetCursorPos(const CVector2& pos)
{
	if (!cursorL)
		return;
	if (((pos.x() + pos.y()) != 0) && !playingPointerMove) {
		Common::PlaySoundStatic(cursorMoveHandle, 800002);
		playingPointerMove = true;
	}
	else if (((pos.x() + pos.y()) == 0) && playingPointerMove)
	{
		playingPointerMove = false;
		cursorMoveHandle.reset();
	}
	posCursorCenter = pos + *LetterboxHelper::ScreenHalfPoint;
	float sizeBox = 100;
	auto posL = CVector2(pos.x() - (sizeBox / 2), pos.y());
	auto posR = CVector2(pos.x() + sizeBox / 2, pos.y());
	auto posT = CVector2(pos.x(), pos.y() + sizeBox / 2);
	auto posB = CVector2(pos.x(), pos.y() - (sizeBox / 2));

	cursorL->SetPosition(posL.x() + LetterboxHelper::ScreenHalfPoint->x(), posL.y() + LetterboxHelper::ScreenHalfPoint->y());
	cursorR->SetPosition(posR.x() + LetterboxHelper::ScreenHalfPoint->x(), posR.y() + LetterboxHelper::ScreenHalfPoint->y());
	cursorT->SetPosition(posT.x() + LetterboxHelper::ScreenHalfPoint->x(), posT.y() + LetterboxHelper::ScreenHalfPoint->y());
	cursorB->SetPosition(posB.x() + LetterboxHelper::ScreenHalfPoint->x(), posB.y() + LetterboxHelper::ScreenHalfPoint->y());
	cursorR->GetNode("arrow_position_9")->SetRotation(180);
	cursorT->GetNode("arrow_position_9")->SetRotation(90);
	cursorB->GetNode("arrow_position_9")->SetRotation(-90);
}
bool IsInsideCursorRange(const CVector2& input, float visibility, int flagID)
{
	if (!cursorL || disabledStick)
		return false;
	bool result = false;
	float range = 100;

	// Check if the point p is on the same side of each edge as the other three corners
	bool inside = abs(Common::GetVector2Distance(input, posCursorCenter)) <= 100;
	if (inside)
	{
		if (visibility >= 80 && flagID != lastFlagIn)
		{
			lastFlagIn = flagID;
			//for eventual night versions, add 9
			cts_guide_ss->GetNode("town_ss_img")->SetPatternIndex(flagID);
			//cts_guide_txt->GetNode("text_size")->SetPatternIndex(flagID);
			cts_name->GetNode("img")->SetPatternIndex(flagID);

			result = true;
		}
	}
	else
	{
		lastFlagIn = -1;
	}
	return result;
}
void SetLevelTextCast(const char* text)
{
	string input = string(text);
	std::stringstream ss(input);
	string line;
	vector<string> lines;
	while (getline(ss, line, '\n')) {
		lines.push_back(line);
	}
	char buff[32];
	for (size_t i = 0; i < 6; i++)
	{
		sprintf(buff, "text_line_%d", i);
		if (i <= lines.size() - 1)
		{
			cts_guide_txt->GetNode(buff)->SetText(lines.at(i).c_str());
			//cts_guide_txt->SetHideFlag(false);
		}
		else
		{
			cts_guide_txt->GetNode(buff)->SetText(" ");
			//cts_guide_txt->SetHideFlag(true);
		}
	}
}

void PopulateStageSelect(int id)
{
	if (Configuration::worldData.data.size() < id)
	{
		printf("\n[WorldMap] Missing config for FlagID %d", id);
		return;
	}
	stageSelectedWindowMax = Configuration::worldData.data[id].data.size() - 1;
	for (size_t i = 0; i < 6; i++)
	{
		deco_text[i]->SetHideFlag(true);
	}

	for (size_t i = 0; i < Configuration::worldData.data[id].data.size(); i++)
	{
		const char* e = Configuration::worldData.data[id].data[i].optionName.c_str();
		deco_text[i]->SetHideFlag(false);
		deco_text[i]->GetNode("Text_blue")->SetText(e);
	}


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
	Chao::CSD::CProject::DestroyScene(rcTitleScreenW.Get(), textWorldDesc);
	Chao::CSD::CProject::DestroyScene(rcTitleScreenW.Get(), cts_guide_4_misson);
	Chao::CSD::CProject::DestroyScene(rcTitleScreenW.Get(), cts_guide_5_medal);

	Chao::CSD::CProject::DestroyScene(rcTitleScreenW.Get(), cts_stage_window);
	Chao::CSD::CProject::DestroyScene(rcTitleScreenW.Get(), cts_stage_select);
	Chao::CSD::CProject::DestroyScene(rcTitleScreenW.Get(), cts_stage_info_window);
	Chao::CSD::CProject::DestroyScene(rcTitleScreenW.Get(), cts_guide_1_hiscore);
	Chao::CSD::CProject::DestroyScene(rcTitleScreenW.Get(), cts_guide_2_besttime);
	Chao::CSD::CProject::DestroyScene(rcTitleScreenW.Get(), cts_guide_3_rank);
	Chao::CSD::CProject::DestroyScene(rcTitleScreenW.Get(), cts_stage_4_misson);
	Chao::CSD::CProject::DestroyScene(rcTitleScreenW.Get(), cts_stage_5_medal);
	for (size_t i = 0; i < 9; i++)
	{
		Chao::CSD::CProject::DestroyScene(rcTitleScreenW.Get(), flag[i]);
	}
	for (size_t i = 0; i < 6; i++)
	{
		Chao::CSD::CProject::DestroyScene(rcTitleScreenW.Get(), deco_text[i]);
	}
	rcTitleScreenW = nullptr;
	introPlayed = false;
	disabledStick = true;
	disabledTarget = true;
	cursorSelected = false;
	playingPan = false;
	timePan = 0;
	camHeight = -20;
	lastAmountSel = 0;
	editorMulti = 1;
	rotationPitch = 0.0f;
	rotationYaw = 0.0f;
	stageSelectedWindow = 0;
	timeStageSelectDelay = 0;
	stageWindowOpen = false;

}
void ShowTextAct(bool visible)
{
	for (size_t i = 0; i < 6; i++)
	{
		deco_text[i]->SetHideFlag(!visible);
	}
}
void SetVisibilityPlayerInfo(bool visible)
{
	infobg1->SetHideFlag(!visible);
	infoimg1->SetHideFlag(!visible);
	infoimg2->SetHideFlag(!visible);
	infoimg3->SetHideFlag(!visible);
	infoimg4->SetHideFlag(!visible);
	cts_guide_4_misson->SetHideFlag(!visible);
	cts_guide_5_medal->SetHideFlag(!visible);
	CSDCommon::FreezeMotion(*cts_guide_ss, 0);
	CSDCommon::FreezeMotion(*cts_guide_txt, 0);
	CSDCommon::FreezeMotion(*cts_guide_window, 0);
	CSDCommon::PlayAnimation(*cts_guide_ss, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, true, !visible);
	CSDCommon::PlayAnimation(*cts_guide_txt, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, true, !visible);
	CSDCommon::PlayAnimation(*cts_guide_window, "Intro_Anim_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, true, !visible);
}
void TitleWorldMap::CreateScreen(Sonic::CGameObject* pParentGameObject)
{
	if (rcTitleScreenW && !spTitleScreenW)
	{
		pParentGameObject->m_pMember->m_pGameDocument->AddGameObject(spTitleScreenW = boost::make_shared<Sonic::CGameObjectCSD>(rcTitleScreenW, 0.5f, "HUD", false), "main", pParentGameObject);

		pParentGameObject->SendMessage(pParentGameObject->m_ActorID, boost::make_shared<Sonic::Message::MsgRequestChangeModule>(6));
		//pParentGameObject->SendMessage(pParentGameObject->m_ActorID, boost::make_shared<Sonic::Message::MsgSetGlobalLightDirection>(CQuaternion(1, 1, 1, 1)));
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


HOOK(int, __fastcall, TitleW_CMain, 0x0056FBE0, Sonic::CGameObject* This, void* Edx, int a2, int a3, void** a4)
{
	CTitleWRemoveCallback(This, nullptr, nullptr);
	CalculateAspectOffsets();



	Sonic::CCsdDatabaseWrapper wrapper(This->m_pMember->m_pGameDocument->m_pMember->m_spDatabase.get());
	auto spCsdProject = wrapper.GetCsdProject("ui_worldmap");
	rcTitleScreenW = spCsdProject->m_rcProject;

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
	cts_guide_4_misson = rcTitleScreenW->CreateScene("cts_guide_4_misson");
	cts_guide_5_medal = rcTitleScreenW->CreateScene("cts_guide_5_medal");

	cts_guide_1_hiscore = rcTitleScreenW->CreateScene("cts_guide_1_hiscore");
	cts_guide_2_besttime = rcTitleScreenW->CreateScene("cts_guide_2_besttime");
	cts_guide_3_rank = rcTitleScreenW->CreateScene("cts_guide_3_rank");
	cts_stage_4_misson = rcTitleScreenW->CreateScene("cts_guide_4_misson");
	cts_stage_5_medal = rcTitleScreenW->CreateScene("cts_guide_5_medal");


	cts_stage_window = rcTitleScreenW->CreateScene("cts_stage_window");
	cts_stage_info_window = rcTitleScreenW->CreateScene("cts_guide_window");
	cts_stage_select = rcTitleScreenW->CreateScene("cts_stage_select");
	cts_name_2 = rcTitleScreenW->CreateScene("cts_name_2");
	stageSelectFlag = rcTitleScreenW->CreateScene("cts_stage_flag");
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


	int pos = 0;
	for (size_t i = 0; i < 6; i++)
	{
		deco_text[i] = rcTitleScreenW->CreateScene("deco_text");
		CSDCommon::PlayAnimation(*deco_text[i], "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		CSDCommon::PlayAnimation(*cts_stage_select, "Select_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, (pos) * 10, pos * 10);
		pos += 1;
		char actname[6];
		sprintf(actname, "Act %d", i + 1);
		deco_text[i]->GetNode("Text_blue")->SetText(actname);
		deco_text[i]->SetPosition(cts_stage_select->GetNode("select_img")->GetPosition().x(), cts_stage_select->GetNode("select_img")->GetPosition().y());
	}
	ShowTextAct(false);

	
	//By default the cursor in the worldmap is set to the left anchor 
	cursorL = rcTitleScreenW->CreateScene("cts_cursor");
	cursorT = rcTitleScreenW->CreateScene("cts_cursor");
	cursorB = rcTitleScreenW->CreateScene("cts_cursor");
	cursorR = rcTitleScreenW->CreateScene("cts_cursor");
	PlayCursorAnim("Intro_Anim");

	//constexpr float earthRadius = 5.575f;
	constexpr float earthRadius = 5.25f; // This looks nicer
	flagPositions.push_back(CVector(0.31f, 0.36f, 2.28f));
	flagPositions.push_back(CVector(2.310000f, 2.360000f, 1.111371f));
	flagPositions.push_back(CVector(2.810000f, -0.140000f, -6.649425f));
	flagPositions.push_back(CVector(2.810000f, -1.890000, 1.742745));
	flagPositions.push_back(CVector(-0.190000, 4.610000, -3.543527));
	flagPositions.push_back(CVector(-5.190000, 0.110000, -3.363136));
	flagPositions.push_back(CVector(0.060000, -2.639999, -6.829812));
	flagPositions.push_back(CVector(-4.440000, -2.390000, -0.798426));
	flagPositions.push_back(CVector(-3.690000, 3.360000, -1.163138)); //-4.440000 y: -2.390000 z : -0.798426
	// Now normalize all these positions, tbh
	for (int i = 0; i < flagPositions.size(); ++i)
	{
		flagPositions.at(i) = ((flagPositions.at(i) - TitleWorldMap::emblemPosition).normalized() * earthRadius) + TitleWorldMap::emblemPosition;
	}

	infoimg1->GetNode("num")->SetText(Common::IntToString(Common::GetLivesCount(), "%02d"));

	TitleWorldMap::CreateScreen(This);
	for (auto e : flag)
	{
		e->SetHideFlag(true);
	}
	SetHideEverythingWM(true);
	cts_guide_4_misson->SetHideFlag(true);
	cts_guide_5_medal->SetHideFlag(true);
	cts_stage_window->SetHideFlag(true);
	cts_stage_select->SetHideFlag(true);
	cts_name_2->SetHideFlag(true);
	stageSelectFlag->SetHideFlag(true);
	cts_stage_info_window->SetHideFlag(true);
	cts_stage_4_misson->SetHideFlag(true);
	cts_stage_5_medal->SetHideFlag(true);
	cts_guide_1_hiscore->SetHideFlag(true);
	cts_guide_2_besttime->SetHideFlag(true);
	cts_guide_3_rank->SetHideFlag(true);


	float sceneFixPosX = 140;
	//Set up scenes to make it work in 16:9, because for some reason unleashed does it in code
	headerIMGW->SetPosition(-sceneFixPosX, 0);
	infobg1->SetPosition(-sceneFixPosX, 0);
	infoimg1->SetPosition(-sceneFixPosX, 0);
	infoimg2->SetPosition(-sceneFixPosX, 0);
	infoimg3->SetPosition(-sceneFixPosX, 0);
	infoimg4->SetPosition(-sceneFixPosX, 0);

	cts_guide_window->SetPosition(sceneFixPosX, 0);
	cts_guide_ss->SetPosition(sceneFixPosX, 0);
	cts_guide_txt->SetPosition(sceneFixPosX, 40);
	cts_guide_4_misson->SetPosition(sceneFixPosX, 0);
	cts_guide_5_medal->SetPosition(sceneFixPosX, 0);

	return originalTitleW_CMain(This, Edx, a2, a3, a4);
}
//void LoadScene(Sonic::CGameObject* a1, const char* stageID)
//{
//	FUNCTION_PTR(Sonic::CGameObject*, __stdcall, sub_D70420, 0xD70420, Sonic::CGameObject * a1);
//	WRITE_MEMORY(0x01A57434, uint32_t, 1);
//	WRITE_MEMORY(0x1A57438, uint32_t, 7);
//	sub_D70420(a1);
//
//}
//void GetRings(Sonic::CGameObject* This, void* Edx)
//{
//	float* gg = 0;
//	auto e = sub_A51F90(sub_A515F0(), 9, gg, Edx);
//	printf("%x", e);
//}
CQuaternion LookRotationFromUnity(CVector direction, CVector up) {
	// Normalize the direction vector
	direction.normalize();

	// Calculate the dot product of the direction vector and the up vector
	float dot = direction.dot(up);

	// Calculate the cross product of the direction vector and the up vector
	CVector cross = direction.cross(up);

	// Normalize the cross product
	cross.normalize();

	// Calculate the angle between the direction vector and the up vector
	float angle = acos(dot);

	// Construct the quaternion using the normalized cross product and the angle
	CQuaternion q = CQuaternion(
		cos(angle / 2),
		sin(angle / 2) * cross.x(),
		sin(angle / 2) * cross.y(),
		sin(angle / 2) * cross.z()
	);
	return q;
}
FUNCTION_PTR(void, __fastcall, DemoAdvance, 0x576470, Sonic::CGameObject* This, void* Edx, int a2);

const char* lastStageID = "";
HOOK(void*, __fastcall, TitleW_UpdateApplication, 0xE7BED0, Sonic::CGameObject* This, void* Edx, float elapsedTime, uint8_t a3)
{
	auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->m_CurrentPadStateIndex];
	float multiplier = 24;
	uint32_t stageTerrainAddress = Common::GetMultiLevelAddress(0x1E66B34, { 0x4, 0x1B4, 0x80, 0x20 });
	char** h = (char**)stageTerrainAddress;
	const char* terr = *h;

	if(terr != lastStageID)
	DebugDrawText::log((std::string("Loading Stage ID:") + std::string(terr)).c_str());

	lastStageID = terr;

	//Pretty much just "if titleworldmap exists"
	if (flag[0] && cts_guide_4_misson && cts_guide_5_medal)
	{
		if(!stageWindowOpen)
		SetCursorPos(CVector2(inputPtr->LeftStickHorizontal * multiplier, -inputPtr->LeftStickVertical * multiplier));		
		//Sonic::CGameDocument::GetInstance()->m_pMember->m_spLightManager->m_GlobalLightDirection = CVector(-79.8565f, 0, 4.78983f);
		uint32_t totalRing = 0;
		for (size_t i = 0; i < 18; i++)
		{
			/*auto stagelist = (const char**)0x01A3EAE8 + i;*/
			uint32_t useless, ringCurrent;
			float uselessF;
			Common::GetStageData(i, useless, uselessF, uselessF, uselessF, useless, ringCurrent, useless);
			totalRing += ringCurrent;
		}

		//GetRings(This, Edx);

		int amountSel = 0;
		if (TitleWorldMap::Camera)
		{
			if (flag[0]->m_MotionDisableFlag && !introPlayed)
			{
				introPlayed = true;
			}
			for (size_t i = 0; i < 9; i++)
			{
				float visibility = fmax(0.0f, -(TitleWorldMap::Camera->m_MyCamera.m_Direction.dot((flagPositions.at(i) - CVector(0, 0, -2.34f)).normalized()))) * 100;
				auto uiPos = WorldToUIPosition(flagPositions.at(i));

				if (introPlayed)
					CSDCommon::PlayAnimation(*flag[i], "Fade_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 0, visibility, visibility);

				flag[i]->SetPosition(uiPos.x(), uiPos.y());
				bool inrange = IsInsideCursorRange(uiPos, visibility, i);
				currentFlagSelected = inrange ? i : -1;
				if (currentFlagSelected != -1)
					lastValidFlagSelected = currentFlagSelected;
				amountSel += inrange;
			}
			if (amountSel != lastAmountSel && currentFlagSelected == lastFlagIn)
			{

				if (amountSel > 0)
				{
					cursorSelected = true;
					CSDCommon::PlayAnimation(*cts_name, "Intro_1_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
					PlayCursorAnim("Select_Anim");
					cts_guide_4_misson->SetHideFlag(false);
					cts_guide_5_medal->SetHideFlag(false);
					CSDCommon::FreezeMotion(*cts_guide_ss, 0);
					CSDCommon::FreezeMotion(*cts_guide_txt, 0);
					CSDCommon::FreezeMotion(*cts_guide_window, 0);
					CSDCommon::PlayAnimation(*cts_guide_ss, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
					CSDCommon::PlayAnimation(*cts_guide_txt, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
					CSDCommon::PlayAnimation(*cts_guide_window, "Intro_Anim_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
					Common::PlaySoundStatic(cursorSelectHandle, 800004);

					SetLevelTextCast(Configuration::worldData.data[lastValidFlagSelected].description.c_str());
					//cts_guide_txt->GetNode("text_size")->SetText(Configuration::worldData.data[lastValidFlagSelected].description.c_str());

				}
				else
				{
					cursorSelected = false;
					PlayCursorAnim("Select_Anim", true);
					cts_guide_4_misson->SetHideFlag(true);
					cts_guide_5_medal->SetHideFlag(true);
					CSDCommon::FreezeMotion(*cts_guide_ss, 0);
					CSDCommon::FreezeMotion(*cts_guide_txt, 0);
					CSDCommon::FreezeMotion(*cts_guide_window, 0);
					CSDCommon::PlayAnimation(*cts_guide_ss, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, true, true);
					CSDCommon::PlayAnimation(*cts_guide_txt, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, true, true);
					CSDCommon::PlayAnimation(*cts_guide_window, "Intro_Anim_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, true, true);
				}
			}
			if (amountSel > 0 && introPlayed)
			{
				if (inputPtr->IsTapped(Sonic::eKeyState_A) && !stageWindowOpen)
				{

					PopulateStageSelect(lastValidFlagSelected);

					cts_stage_window->SetHideFlag(false);
					cts_stage_select->SetHideFlag(false);
					cts_name_2->SetHideFlag(false);
					stageSelectFlag->SetHideFlag(false);
					stageWindowOpen = true;
					CSDCommon::PlayAnimation(*cts_stage_window, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
					CSDCommon::PlayAnimation(*cts_name_2, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
					CSDCommon::PlayAnimation(*cts_stage_select, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
					CSDCommon::PlayAnimation(*stageSelectFlag, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
					cts_name_2->GetNode("img_1")->SetPatternIndex(lastValidFlagSelected);
					stageSelectFlag->GetNode("img")->SetPatternIndex(lastValidFlagSelected);
					stageSelectedWindow = 0;
					CSDCommon::FreezeMotion(*cts_stage_select, 0);
					Common::PlaySoundStatic(stageSelectHandle, 2);

					SetVisibilityPlayerInfo(false);
					cts_stage_info_window->SetHideFlag(false);

					cts_guide_1_hiscore->SetHideFlag(false);
					cts_guide_2_besttime->SetHideFlag(false);
					cts_guide_3_rank->SetHideFlag(false);
					//cts_stage_4_misson->SetHideFlag(false);
					cts_stage_5_medal->SetHideFlag(false);
					CSDCommon::PlayAnimation(*cts_stage_info_window, "Intro_Anim_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, true, false);
				}
				if (inputPtr->IsTapped(Sonic::eKeyState_B) && stageWindowOpen)
				{
					stageWindowOpen = false;
					CSDCommon::PlayAnimation(*cts_stage_window, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, false, true);
					CSDCommon::PlayAnimation(*cts_name_2, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, false, true);
					cts_stage_select->SetHideFlag(true);
					stageSelectFlag->SetHideFlag(true);
					timeStageSelectDelay = 0;
					ShowTextAct(false);
					Common::PlaySoundStatic(stageSelectHandle, 4);
					SetVisibilityPlayerInfo(true);
					cts_guide_1_hiscore->SetHideFlag(true);
					cts_guide_2_besttime->SetHideFlag(true);
					cts_guide_3_rank->SetHideFlag(true);
					cts_stage_4_misson->SetHideFlag(true);
					cts_stage_5_medal->SetHideFlag(true);
					CSDCommon::PlayAnimation(*cts_stage_info_window, "Intro_Anim_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, true, true);
				}
				if (stageWindowOpen)
				{
					if (timeStageSelectDelay < 5)
						timeStageSelectDelay += 1;
					if (inputPtr->IsTapped(Sonic::eKeyState_A) && timeStageSelectDelay >= 5)
					{
						Title::canLoad = 1;
					}
					if (inputPtr->IsTapped(Sonic::eKeyState_LeftStickDown) && stageSelectedWindow != stageSelectedWindowMax)
					{
						stageSelectHandle.reset();
						Common::PlaySoundStatic(stageSelectHandle, 0);
							stageSelectedWindow += 1;
						CSDCommon::PlayAnimation(*cts_stage_select, "Select_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, (stageSelectedWindow - 1) * 10, stageSelectedWindow * 10);
					}
					if (inputPtr->IsTapped(Sonic::eKeyState_LeftStickUp))
					{
						Common::PlaySoundStatic(stageSelectHandle, 0);
						stageSelectedWindow -= 1;
						if (stageSelectedWindow < 5)
							CSDCommon::PlayAnimation(*cts_stage_select, "Select_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 130 - ((stageSelectedWindow + 1) * 10), 130 - (stageSelectedWindow) * 10);
					}
					Common::ClampInt(stageSelectedWindow, 0, stageSelectedWindowMax);
				}
			}
			cts_name->SetHideFlag(!cursorSelected);
			lastAmountSel = amountSel;

		}


		CheckCursorAnimStatus();
	}
	return originalTitleW_UpdateApplication(This, Edx, elapsedTime, a3);
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
float VectorAngle(const CVector& a, const CVector& b)
{
	const float dot = a.dot(b);
	//return acos(dot / sqrt(a.squaredNorm() * b.squaredNorm()));
	return acos(dot / sqrt(a.squaredNorm()));
}
inline float lerpUnclampedf(const float a, const float b, const float t)
{
	return a + (b - a) * t;
}
inline float lerpf(float a, float b, float t)
{
	const float min = fmin(a, b);
	const float max = fmax(a, b);
	return fmin(min, fmax(max, lerpUnclampedf(a, b, t)));
}
void MagnetizeToFlag(const CVector& flagPosition, float deltaTime)
{
	// Helpful thing here
	constexpr float halfway = (180.0f * DEG2RAD);
	// First, we need to convert our flag position to target radians.
	// Reminder: Yaw increases when rotating to the right, Pitch is negative facing down & positive facing up.
	// Pitch is easy. Get the angle in radians, then subtract by half max.
	const float rPitch = VectorAngle(flagPosition, CVector(0, 1, 0)) - (90.0f * DEG2RAD);
	// Yaw is tricky due to the wraparound, and because it's a planar rotation.
	// This means we actually need 0 -> 360, which involves a few steps.
	// We can't just do an angle check with forward. We need a PLANAR vector to compare with.
	// TODO: These math operations are getting expensive, so we need to really pre-compute the radians we want to use.
	const CVector planarPosition = CVector(flagPosition.x(), 0, flagPosition.z()).normalized();
	const float rInitialYaw = VectorAngle(planarPosition, CVector(0, 0, 1));
	// Our angle is an unsigned angle from 0 -> 180. We need to convert that to 0 -> 360, and... well this is how you do that lol
	const float rCorrectedYaw = flagPosition.dot(CVector(-1, 0, 0)) > 0.0f
		? (halfway - rInitialYaw) + halfway
		: rInitialYaw;
	// Now, we want to do some bullshit to make the radian lerp-towards work.
	const bool isOverfill = fabs(rotationYaw - rCorrectedYaw) > halfway;
	const bool isFlagLeft = rotationYaw > halfway;
	float rYaw = rCorrectedYaw;
	if (isOverfill)
	{
		const float compensation = (isFlagLeft ? 360.0f : -360.0f) * DEG2RAD;
		rYaw += compensation;
	}
	// Time to lerp!
	constexpr float lerpRate = 2.5f; // I like 3.0f, but if we want this to be more like Unleashed's, 2.5f feels about right.
	rotationPitch = lerpUnclampedf(rotationPitch, rPitch, deltaTime * lerpRate);
	rotationYaw = lerpUnclampedf(rotationYaw, rYaw, deltaTime * lerpRate);
}


HOOK(void, __fastcall, Title_CameraUpdate, 0x0058CDA0, TransitionTitleCamera* This, void* Edx, const Hedgehog::Universe::SUpdateInfo& updateInfo)
{
	using namespace hh::math;
	auto* const camera = This->m_spCamera.get();
	TitleWorldMap::Camera = camera;
	if (!camera)
		originalTitle_CameraUpdate(This, Edx, updateInfo);

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

	const CVector2 pan(input.LeftStickHorizontal, input.LeftStickVertical);
	const float deadzone = 0.1f; // TODO: MAKE PARAMETER? USE SOMETHING IN GENS?


	const bool hasInput = pan.squaredNorm() > deadzone * deadzone;

	if (!disabledStick && hasInput && !stageWindowOpen)
	{
		rotationPitch -= input.LeftStickVertical * rotationPitchRate * updateInfo.DeltaTime;
		rotationYaw += input.LeftStickHorizontal * rotationYawRate * updateInfo.DeltaTime;
	}
	// Do the thing where we magnetize our input.
	// TODO: Handle the HUD update here too I guess, because this is where a flag will be "selected"
	// Otherwise, make that handled somewhere else. I know you do an overlap check already, but best to do this once.
	else
	{
		constexpr float dotThreshold = 0.95f; // Value I determined to work pretty well.
		for (CVector position : flagPositions )
		{
			const CVector direction = (position - TitleWorldMap::emblemPosition).normalized();
			if (-direction.dot(camera->m_MyCamera.m_Direction) < dotThreshold && currentFlagSelected == -1 )
				continue;
			if(introPlayed)
			MagnetizeToFlag(direction, updateInfo.DeltaTime);
			break;
		}
	}

	// Gotta do this nonsense.
	// UNDONE: This method wouldn't let us select Holoska, & the bottom of earth has nothing right now.
	//constexpr float pitchMaxExtents = 70.0f * DEG2RAD; // Max rotation is 70 degrees in either direction,
	//                                                   // rather than 90, which would get us to the poles of the earth.


	// Min and max extents configured differently so we aren't aimlessly rotating around the south pole (TBD)
	// and so we can actually select Holoska correctly.
	constexpr float pitchBtm = 50.0f * DEG2RAD;
	constexpr float pitchTop = 75.0f * DEG2RAD;

	// Now limit
	rotationPitch = fmax(-pitchTop, fmin(rotationPitch, pitchBtm));
	// Cycle yaw so it doesn't go over 360, so we don't approach Very Large Numbers.
	rotationYaw = WrapAroundFloat(rotationYaw, 360.0 * DEG2RAD);

	const CQuaternion pitch = TitleWorldMap::QuaternionFromAngleAxis(rotationPitch, CVector(1, 0, 0));
	const CQuaternion yaw = TitleWorldMap::QuaternionFromAngleAxis(rotationYaw, CVector(0, 1, 0));

	const CQuaternion rotation = yaw * pitch;

	if (playingPan)
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

void SetCorrectStageFromFlag()
{
	uint32_t stageTerrainAddress = Common::GetMultiLevelAddress(0x1E66B34, { 0x4, 0x1B4, 0x80, 0x20 });
	char** h = (char**)stageTerrainAddress;
	const char* stageToLoad = "ghz200";
	if (Configuration::worldData.data.size() < lastValidFlagSelected)
	{
		//if only cpp had the same ${} system as c#
		std::string message = "This country has an invalid configuration. Loading " + std::string(stageToLoad) + " instead.";
		MessageBoxA(NULL, message.c_str(), "Unleashed Title Screen", 0);
		printf("\n[WorldMap] Missing config for FlagID %d", lastValidFlagSelected);
	}
	else
		stageToLoad = Configuration::worldData.data[lastValidFlagSelected].data[stageSelectedWindow].levelID.c_str();


	strcpy(*(char**)stageTerrainAddress, stageToLoad);
}

void __declspec(naked) SetCorrectTerrainForMission_ASM()
{
	static uint32_t sub_662010 = 0x662010;
	static uint32_t returnAddress = 0xD56CCF;
	__asm
	{
		call[sub_662010]
		push    esi
		call    SetCorrectStageFromFlag
		pop     esi
		jmp[returnAddress]
	}
}
void __declspec(naked) SetGameplayFlowMode()
{
	//yeah this could be a CMP, i just dont know how i would do it with such a long value
	static uint32_t normal = 0x00D0A7E0;
	static uint32_t whiteWorld = 0x00D0A746;

	static bool pamb = Configuration::worldData.data[lastValidFlagSelected].data[stageSelectedWindow].isWhiteWorld;

		__asm
		{
 			jmp[normal]
		}
}

int test = 0;
HOOK(void, __fastcall, LoadAR, 0x0069C270, DWORD* This, DWORD* a2, Hedgehog::Base::CSharedString a3, Hedgehog::Base::CSharedString* a4, DWORD* a5, void* Edx)
{
	if (test == 0)
	{
		Hedgehog::Base::CSharedString og = *a4;
		Hedgehog::Base::CSharedString newn = Hedgehog::Base::CSharedString("EnemyBeeton.ar");
		a4 = &newn;
		originalLoadAR(This, a2, a3, a4, a5, Edx);
		test += 1;
		a4 = &og;
	}
	originalLoadAR(This, a2, a3, a4, a5, Edx);
}

HOOK(char, __fastcall, GoalOutro, 0x00D078B0, byte* a2, DWORD* a3, DWORD* This)
{
	auto v3 = This[4];
	
	if (v3 == 10)
	{

	}

	return originalGoalOutro(a2, a3, This);
}
void __declspec(naked) SetHUDMode()
{
	static uint32_t pam = 0x00D907B3;
	static uint32_t normal = 0x00D9079F;
	static bool pamb = Configuration::worldData.data[lastValidFlagSelected].data[stageSelectedWindow].isWhiteWorld;
	__asm
	{
		cmp pamb, 1
		je PamHUD
		jmp[normal]
		PamHUD:
		jmp[pam]
	}
	/*}*/

}
void __declspec(naked) SetFlowMode()
{
	static uint32_t pam = 0x00582391;
	static uint32_t normal = 0x005823B8;
	static bool pamb = Configuration::worldData.data[lastValidFlagSelected].data[stageSelectedWindow].isWhiteWorld;
	__asm
	{
		cmp pamb, 1
		je PamHUD
		jmp[normal]
		PamHUD:
		push    esi
		jmp[pam]
	}
	/*}*/

}


void TitleWorldMap::Install()
{
	//WRITE_MEMORY(0xD77102, uint32_t, 2);
	//WRITE_MEMORY(0xD7712E, uint32_t, 2);
	//INSTALL_HOOK(TitleUI_CDemoMenuObjectAdvance);
	WRITE_JUMP(0xD56CCA, SetCorrectTerrainForMission_ASM);
	WRITE_JUMP(0x00D9078D, SetHUDMode); //Skip loading PAM000 HUD
	WRITE_JUMP(0x00582390, SetFlowMode); //Completely skip initializing CGameplayFlowPlayableMenu

	WRITE_JUMP(0x00D0DFA0, 0x00D0DDC0);//Try to load title instead (does nothing)
	WRITE_JUMP(0x010A0B4A, 0x010A0B5A); //Force Act pause menu
	WRITE_JUMP(0x00584CEE, 0x00588820);
	WRITE_JUMP(0x00D0A743, SetGameplayFlowMode);
	//WRITE_JUMP(0x00CF8E08, TestPam);

	//WRITE_JUMP(0x015B84F8, 0x015B84FC);
	/*WRITE_JUMP(0x010B978C, 0x010B984D);*/
	//INSTALL_HOOK(TitleWM_CMain_CState_SelectMenu);
	//INSTALL_HOOK(sub_571170);
	INSTALL_HOOK(LoadAR);
	INSTALL_HOOK(GoalOutro);
	INSTALL_HOOK(Title_CameraUpdate);
	INSTALL_HOOK(TitleW_CMain);
	INSTALL_HOOK(TitleW_UpdateApplication);
	WRITE_MEMORY(0x016E11F4, void*, CTitleWRemoveCallback);
}