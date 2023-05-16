
//	TODO:
//	Make sun gameobject to spawn the particle that Qution made
//	Make passing-time mechanic, along with being able to tell if a flag is in the shade or not
//	Add proper white-world support

//	TODO for Infinite Levels:
//	Fix MsgResultChangeState from not playing
//	Fix GoalRing redirecting you to the same stage or another stage (it loads pam000 but it gets replaced)
//	Fix Enemy archives not loading via archive tree

struct FlagUIInformation
{
	Chao::CSD::RCPtr<Chao::CSD::CScene> flag;
	Chao::CSD::RCPtr<Chao::CSD::CScene> sun_moon;
	int id;
	bool night, playingMedalTransition;
	float visibility;
};
//vs shits itself if these are in pch, no idea why
constexpr double RAD2DEG = 57.29578018188477;
constexpr double DEG2RAD = 0.01745329238474369; 
using namespace hh::math;
class CObjDropRing : public Sonic::CGameObject3D
{
	hh::math::CMatrix44 m_Transform;
	hh::math::CQuaternion m_Rotation;
	hh::math::CQuaternion m_TargetRotation;

	float m_LifeTime{};
	boost::shared_ptr<hh::mr::CSingleElement> m_spModel;

public:
	CObjDropRing()
	{
		const auto spCamera = Sonic::CGameDocument::GetInstance()->GetWorld()->GetCamera();
		const hh::math::CMatrix viewTransform = spCamera->m_MyCamera.m_View * hh::math::CMatrix::Identity();
		m_TargetRotation = viewTransform.rotation().inverse();
		m_Transform = spCamera->m_MyCamera.m_Projection * viewTransform.matrix();
		m_Transform.normalize();
		m_Rotation = m_TargetRotation;
	}

	void AddCallback(const Hedgehog::Base::THolder<Sonic::CWorld>& worldHolder, Sonic::CGameDocument* pGameDocument,
		const boost::shared_ptr<Hedgehog::Database::CDatabase>& spDatabase) override
	{
		Sonic::CApplicationDocument::GetInstance()->AddMessageActor("GameObject", this);
		pGameDocument->AddUpdateUnit("f", this);

		m_spModel = boost::make_shared<hh::mr::CSingleElement>(hh::mr::CMirageDatabaseWrapper(spDatabase.get()).GetModelData("title_parts"));		
		m_LifeTime = 0.0f;
	}

	void UpdateParallel(const Hedgehog::Universe::SUpdateInfo& updateInfo) override
	{
		/*const auto spCamera = m_pMember->m_pGameDocument->GetWorld()->GetCamera();

		const hh::math::CMatrix44 invProj = spCamera->m_MyCamera.m_Projection.inverse();
		const hh::math::CMatrix invView = spCamera->m_MyCamera.m_View.inverse();

		constexpr float gravity = -9.81f;
		Hedgehog::Math::CVector2 const velPrev = m_2DVelocity;
		m_2DVelocity += Hedgehog::Math::CVector2::UnitY() * gravity * updateInfo.DeltaTime;
		m_2DPosition += (velPrev + m_2DVelocity) * 0.5f * updateInfo.DeltaTime;

		auto& rTransform = m_spModel->m_spInstanceInfo->m_Transform;
		auto& rMatrix = rTransform.matrix();

		rTransform = m_Transform;

		const float scale = max(rMatrix.col(0).head<3>().norm(),
			max(rMatrix.col(1).head<3>().norm(), rMatrix.col(2).head<3>().norm()));

		rMatrix.col(0) /= (scale / 0.2f);
		rMatrix.col(1) /= (scale / 0.2f);
		rMatrix.col(2) /= (scale / 0.2f);

		rTransform(0, 3) = m_2DPosition.x();
		rTransform(1, 3) = m_2DPosition.y();
		rTransform(2, 3) = 0.1f;
		rTransform(3, 3) = 1.0f;

		rTransform = invProj * rTransform.matrix();
		rTransform.rotate(m_Rotation);
		rTransform = invView * rTransform;

		(&rTransform)[1] = rTransform;

		m_Rotation = m_TargetRotation;

		m_LifeTime += updateInfo.DeltaTime;
		if (m_LifeTime > 2.0f)
		{
			SendMessage(m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
		}*/
	}

	bool ProcessMessage(Hedgehog::Universe::Message& message, bool flag) override
	{
		if (flag)
		{
			if (std::strstr(message.GetType(), "MsgRestartStage") != nullptr
				|| std::strstr(message.GetType(), "MsgStageClear") != nullptr)
			{
				SendMessage(m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
				return true;
			}
		}

		return Sonic::CGameObject::ProcessMessage(message, flag);
	}
};
bool  TitleWorldMap::LoadingReplacementEnabled= true;
bool  TitleWorldMap::ForceLoadToFlowTitle = false;
Chao::CSD::RCPtr<Chao::CSD::CProject> rcWorldMap;
boost::shared_ptr<Sonic::CGameObjectCSD> spWorldMap;
Chao::CSD::RCPtr<Chao::CSD::CScene> infobg1, worldmap_footer_img_A, infoimg1, infoimg2, infoimg3, infoimg4, headerBGW, headerIMGW, footerBGW, footerIMGW;
Chao::CSD::RCPtr<Chao::CSD::CScene> cursorL, cursorT, cursorB, cursorR;
Chao::CSD::RCPtr<Chao::CSD::CScene> cts_name, cts_cursor_effect, cts_guide_window, cts_guide_ss, cts_guide_txt, cts_guide_4_misson, cts_guide_5_medal;
Chao::CSD::RCPtr<Chao::CSD::CScene> cts_stage_window, cts_stage_select, cts_name_2, stageSelectFlag, cts_stage_info_window, cts_guide_1_hiscore, cts_guide_2_besttime, cts_guide_3_rank, cts_stage_4_misson, cts_stage_5_medal, cts_stage_ss;
Chao::CSD::RCPtr<Chao::CSD::CScene> cts_choices_bg, cts_choices_window, cts_choices_select;


FlagUIInformation flag[9];
//	theres defo a better way to handle this
bool isInShade[9];
bool sunMoonPlayingTransition[9];
//
Chao::CSD::RCPtr<Chao::CSD::CScene> deco_text[8];
Chao::CSD::RCPtr<Chao::CSD::CScene> textWorldDesc, textStageDesc;

std::vector<CVector> flagPositions; 
static AudioHandle cursorMoveHandle, cursorSelectHandle, stageSelectHandle, worldMapMusicHandle;
const CVector TitleWorldMap::emblemPosition = CVector(0, 0, -2.34f);
const CustomCamera* TitleWorldMap::Camera;
CVector2 posCursorCenter;
CVector2* offsetAspect;
CVector2* offsetRes;


static bool isWhiteWorldEnabled = false;
static float rotationPitch = 20.0f;
static float FOV = 0.84906584f;
static float rotationYaw = 0.0f;
int stageSelectedWindow = 0;
int stageSelectedWindowMax = 6;
int flagSelectionAmount = 0;

int selectedCapital = 0;
bool stageWindowOpen, capitalWindowOpen = false;
bool playingPointerMove;
bool introPlayed = false;
bool disabledStick = true;
bool disabledTarget = true;
bool cursorSelected = false;
bool playingPan = false;
float timePan, timeStageSelectDelay = 0;
float camHeight = -20;
float lastflagSelectionAmount = 0;
float editorMulti = 1;
int currentFlagSelected, lastValidFlagSelected, lastFlagIn = 0;
hh::fnd::CStateMachineBase::CStateBase* testState;
boost::shared_ptr<Sonic::CLightManager> light;
const char* lastStageID = "";
boost::shared_ptr<Sonic::CGameObject3D> earth;
bool TitleWorldMap::Active = false;
float multiplierRotationLight = 0.1f;


static bool camInitialized = false;
//From Brianuu's 06 Title
boost::shared_ptr<SaveLoadTestStruct> m_spSave;
FUNCTION_PTR(void, __thiscall, TitleWorldMap_CTitleOptionCStateOutroSaving, 0xD22A70, boost::shared_ptr<SaveLoadTestStruct>& spSave, void* a2);
void fpAddParticle(DWORD* manager, void* handle, const boost::shared_ptr<Sonic::CMatrixNodeTransform>& node, const hh::base::CSharedString& name, uint32_t flag)
{
	uint32_t func = 0x00E8F8A0;
	__asm
	{
		push flag
		push name
		push node
		push handle
		mov eax, manager
		call func
	};
};
Sonic::CGameObject* PauseInitTest(Sonic::CGameObject* objectt, int PAUSEtype)
{
	uint32_t func = 0x010A1A30;
	__asm
	{
		mov		eax, objectt;
		mov		ecx, PAUSEtype;
		push    ecx;
		call    func;
		retn
	};
};

void SetHideEverythingWM(bool hide)
{
	infobg1->SetHideFlag(hide);
	worldmap_footer_img_A->SetHideFlag(hide);
	infoimg1->SetHideFlag(hide);
	infoimg2->SetHideFlag(hide);
	infoimg3->SetHideFlag(hide);
	infoimg4->SetHideFlag(hide);
	headerBGW->SetHideFlag(hide);
	headerIMGW->SetHideFlag(hide);
	footerBGW->SetHideFlag(hide);
	if(footerIMGW)
	footerIMGW->SetHideFlag(hide);
	cursorL->SetHideFlag(hide);
	cursorT->SetHideFlag(hide);
	cursorB->SetHideFlag(hide);
	cursorR->SetHideFlag(hide);
	cts_name->SetHideFlag(hide);
	cts_cursor_effect->SetHideFlag(hide);

	cts_guide_ss->SetHideFlag(hide);
	cts_guide_window->SetHideFlag(hide);
	cts_guide_txt->SetHideFlag(hide);
	
	for (size_t i = 0; i < 9; i++)
	{
		flag[i].flag->SetHideFlag(hide);
		flag[i].sun_moon->SetHideFlag(hide);
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
	if (!cursorL || !TitleWorldMap::Active)
		return;
	if (((pos.x() + pos.y()) != 0) && !playingPointerMove) 
	{
		MiniAudioHelper::PlaySound(cursorMoveHandle, 10, "PointerMove", true);
		//Common::MiniAudioHelper::PlaySoundStatic(cursorMoveHandle, 10);
		playingPointerMove = true;
	}
	else if (((pos.x() + pos.y()) == 0) && playingPointerMove)
	{
		playingPointerMove = false;
		MiniAudioHelper::StopSound(cursorMoveHandle);
		//cursorMoveHandle.reset();
	}
	posCursorCenter = pos + *LetterboxHelper::ScreenHalfPoint;
	float sizeBox = 125;
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
	bool inside = abs(Common::GetVector2Distance(input, posCursorCenter)) <= 50;
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
	if (lines.size() == 0)
		lines.push_back("");
	char buff[32];
	for (size_t i = 0; i < 6; i++)
	{
		sprintf(buff, "text_line_%d", i);
		if (i <= lines.size() - 1)
		{
			cts_guide_txt->GetNode(buff)->SetText(lines.at(i).c_str());
		}
		else
		{
			cts_guide_txt->GetNode(buff)->SetText(" ");
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
	stageSelectedWindowMax = flag[id].night && Configuration::worldData.data[id].dataNight.size() != 0 ? Configuration::worldData.data[id].dataNight.size() - 1 : Configuration::worldData.data[id].data.size() - 1;
	Common::ClampInt(stageSelectedWindowMax, 0, 6);
	for (size_t i = 0; i < 6; i++)
	{
		deco_text[i]->SetHideFlag(true);
	}

	for (size_t i = 0; i < 6; i++)
	{
		if (stageSelectedWindowMax < i)
			break;

		const char* optionName;
		if(flag[id].night && Configuration::worldData.data[id].dataNight.size() != 0)
			optionName = Configuration::worldData.data[id].dataNight[i].optionName.c_str();
		else
			optionName = Configuration::worldData.data[id].data[i].optionName.c_str();

		deco_text[i]->SetHideFlag(false);
		deco_text[i]->GetNode("Text_blue")->SetText(optionName);
	}
}
TitleWorldMap::SaveStageInfo GetInfoForStage(std::string id)
{
	vector<string>::iterator it = std::find(Configuration::gensStages.begin(), Configuration::gensStages.end(), id);
	TitleWorldMap::SaveStageInfo returnI = TitleWorldMap::SaveStageInfo();
	returnI.stageID_string = id;
	//Return an empty Info struct if the stage isnt a native gens stage. This should be expanded to have custom stage slot save files in the future.
	if (it == Configuration::gensStages.end())
		return returnI;

	returnI.stageIDForGens = std::distance(Configuration::gensStages.begin(), it);
	returnI.isStageCompleted = Common::IsStageCompleted(returnI.stageIDForGens);
	Common::GetStageData(returnI.stageIDForGens, returnI.bestScore, returnI.bestTime, returnI.bestTime2, returnI.bestTime3, returnI.bestRank, returnI.bestRing, returnI.redRingCount);
	return returnI;
}
void PopulateCountryPreviewInfo(int flag)
{
	int redRingMax = 5 * Configuration::worldData.data[flag].data.size();
	int redRingCurrent = 0;
	int stageCompletedAmount = 0;
	for (size_t i = 0; i < Configuration::worldData.data[flag].data.size() ; i++)
	{
		auto e = GetInfoForStage(Configuration::worldData.data[flag].data[i].levelID);
		redRingCurrent += e.redRingCount;
		stageCompletedAmount += e.isStageCompleted;
	}
	char* redRingsCurrent = new char[8];
	char* redRingsMax = new char[8];
	char* stageMax = new char[8];
	char* stageCount = new char[8];
	sprintf(redRingsCurrent, "%02d", redRingCurrent);
	sprintf(redRingsMax, "%02d", redRingMax);
	sprintf(stageMax, "%02d", Configuration::worldData.data[flag].data.size());
	sprintf(stageCount, "%02d", stageCompletedAmount);

	cts_guide_4_misson->GetNode("num_nume")->SetText(stageCount);
	cts_guide_4_misson->GetNode("num_deno")->SetText(stageMax);

	cts_guide_5_medal->GetNode("s_num_nume")->SetText(redRingsCurrent);
	cts_guide_5_medal->GetNode("s_num_deno")->SetText(redRingsMax);

	cts_guide_5_medal->GetNode("m_num_nume")->SetText(redRingsCurrent);
	cts_guide_5_medal->GetNode("m_num_deno")->SetText(redRingsMax);
}
void PopulateStageWindowInfo(std::string id)
{
	auto info = GetInfoForStage(id);

	// Time
	uint32_t minutes, seconds, milliseconds;
	uint32_t totalMilliseconds = info.bestTime * 1000.0f;
	minutes = totalMilliseconds / 60000;
	if (info.bestTime > 0.0f && minutes <= 99)
	{
		seconds = (totalMilliseconds % 60000) / 1000;
		milliseconds = (totalMilliseconds % 60000) % 1000;
	}
	else
	{
		minutes = 00;
		seconds = 00;
		milliseconds = 000;
	}
	char* scoreCount = new char[8];
	char* redCount = new char[2];
	char* bestTimeC = new char[16];
	sprintf(bestTimeC, "%02d:%02d:%02d", minutes, seconds, milliseconds);
	sprintf(scoreCount, "%08d", info.bestScore);
	sprintf(redCount, "%02d", info.redRingCount);

	cts_guide_2_besttime->GetNode("num")->SetText(bestTimeC);
	cts_guide_1_hiscore->GetNode("num")->SetText(scoreCount);
	cts_guide_3_rank->GetNode("rank_shade")->SetPatternIndex(info.bestRank);
	cts_guide_3_rank->GetNode("rank_img")->SetPatternIndex(info.bestRank);

	cts_stage_5_medal->GetNode("s_num_nume")->SetText(redCount);
	cts_stage_5_medal->GetNode("s_num_deno")->SetText("05");

	cts_stage_5_medal->GetNode("m_num_nume")->SetText(redCount);
	cts_stage_5_medal->GetNode("m_num_deno")->SetText("05");
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
Sonic::CGameObject* teoqr;
void TitleWorldMap::Start()
{
	//Init Anims
	TitleWorldMap::Active = true;
	TitleWorldMap::LoadingReplacementEnabled = true;
	TitleWorldMap::ForceLoadToFlowTitle = false;
	TitleWorldMap_CTitleOptionCStateOutroSaving(m_spSave, nullptr);
	//Set lives text
	infoimg1->GetNode("num")->SetText(Common::IntToString(Common::GetLivesCount(), "%02d"));
	int test = (int)(((Sonic::CApplicationDocument*)0x01E66B34) + 0x4C);
	introPlayed = false;
	SetHideEverythingWM(false);
	PlayCursorAnim("Intro_Anim");
	CSDCommon::PlayAnimation(*infobg1, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*infoimg1, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*infoimg2, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*infoimg3, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*infoimg4, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*headerBGW, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 60); //60 so it doesnt fade in from the previous header and footer of HudTitle
	CSDCommon::PlayAnimation(*footerBGW, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 60); //60 so it doesnt fade in from the previous header and footer of HudTitle
	CSDCommon::PlayAnimation(*headerIMGW, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*cts_guide_ss, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*cts_guide_txt, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*cts_guide_window, "Intro_Anim_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*worldmap_footer_img_A, "Intro_Anim_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);

	PlayCursorAnim("Intro_Anim");
	for (size_t i = 0; i < 9; i++)	CSDCommon::PlayAnimation(*flag[i].flag, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	
	Sonic::CGameDocument::GetInstance()->AddGameObject(earth = boost::make_shared<CObjDropRing>());
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
	Chao::CSD::CProject::DestroyScene(rcWorldMap.Get(), infobg1);
	Chao::CSD::CProject::DestroyScene(rcWorldMap.Get(), infoimg2);
	Chao::CSD::CProject::DestroyScene(rcWorldMap.Get(), infoimg3);
	Chao::CSD::CProject::DestroyScene(rcWorldMap.Get(), infoimg4);
	Chao::CSD::CProject::DestroyScene(rcWorldMap.Get(), headerBGW);
	Chao::CSD::CProject::DestroyScene(rcWorldMap.Get(), headerIMGW);
	Chao::CSD::CProject::DestroyScene(rcWorldMap.Get(), footerBGW);
	Chao::CSD::CProject::DestroyScene(rcWorldMap.Get(), footerIMGW);
	Chao::CSD::CProject::DestroyScene(rcWorldMap.Get(), worldmap_footer_img_A);

	Chao::CSD::CProject::DestroyScene(rcWorldMap.Get(), cursorL);
	Chao::CSD::CProject::DestroyScene(rcWorldMap.Get(), cursorT);
	Chao::CSD::CProject::DestroyScene(rcWorldMap.Get(), cursorB);
	Chao::CSD::CProject::DestroyScene(rcWorldMap.Get(), cursorR);

	Chao::CSD::CProject::DestroyScene(rcWorldMap.Get(), cts_name);
	Chao::CSD::CProject::DestroyScene(rcWorldMap.Get(), cts_cursor_effect);
	Chao::CSD::CProject::DestroyScene(rcWorldMap.Get(), cts_guide_ss);
	Chao::CSD::CProject::DestroyScene(rcWorldMap.Get(), cts_stage_ss);
	Chao::CSD::CProject::DestroyScene(rcWorldMap.Get(), cts_guide_window);
	Chao::CSD::CProject::DestroyScene(rcWorldMap.Get(), cts_guide_txt);
	Chao::CSD::CProject::DestroyScene(rcWorldMap.Get(), textWorldDesc);
	Chao::CSD::CProject::DestroyScene(rcWorldMap.Get(), cts_guide_4_misson);
	Chao::CSD::CProject::DestroyScene(rcWorldMap.Get(), cts_guide_5_medal);

	Chao::CSD::CProject::DestroyScene(rcWorldMap.Get(), cts_stage_window);
	Chao::CSD::CProject::DestroyScene(rcWorldMap.Get(), cts_stage_select);
	Chao::CSD::CProject::DestroyScene(rcWorldMap.Get(), cts_stage_info_window);
	Chao::CSD::CProject::DestroyScene(rcWorldMap.Get(), cts_guide_1_hiscore);
	Chao::CSD::CProject::DestroyScene(rcWorldMap.Get(), cts_guide_2_besttime);
	Chao::CSD::CProject::DestroyScene(rcWorldMap.Get(), cts_guide_3_rank);
	Chao::CSD::CProject::DestroyScene(rcWorldMap.Get(), cts_stage_4_misson);
	Chao::CSD::CProject::DestroyScene(rcWorldMap.Get(), cts_stage_5_medal);

	Chao::CSD::CProject::DestroyScene(rcWorldMap.Get(), cts_choices_bg);
	Chao::CSD::CProject::DestroyScene(rcWorldMap.Get(), cts_choices_window);
	Chao::CSD::CProject::DestroyScene(rcWorldMap.Get(), cts_choices_select);

	for (size_t i = 0; i < 9; i++)
	{
		Chao::CSD::CProject::DestroyScene(rcWorldMap.Get(), flag[i].flag);
		Chao::CSD::CProject::DestroyScene(rcWorldMap.Get(), flag[i].sun_moon);
	}
	for (size_t i = 0; i < 8; i++)	Chao::CSD::CProject::DestroyScene(rcWorldMap.Get(), deco_text[i]);

	rcWorldMap = nullptr;

	timePan = 0;
	camHeight = -20;
	editorMulti = 1;
	playingPan = false;
	introPlayed = false;
	disabledStick = true;
	rotationPitch = -0.4f;
	FOV = 0.84906584f;
	rotationYaw = 0.55f;
	disabledTarget = true;
	cursorSelected = false;
	stageSelectedWindow = 0;
	stageWindowOpen = false;
	timeStageSelectDelay = 0;
	lastflagSelectionAmount = 0;

}
const char* GetStageToLoad()
{
	const char* stageToLoad = "ghz200";
	if (capitalWindowOpen)
		stageToLoad = Configuration::worldData.data[lastValidFlagSelected].data[Configuration::GetCapital(lastValidFlagSelected)].levelID.c_str();
	else
	{
		if (Configuration::worldData.data[lastValidFlagSelected].dataNight.size() != 0 && flag[lastValidFlagSelected].night)
			stageToLoad = Configuration::worldData.data[lastValidFlagSelected].dataNight[stageSelectedWindow].levelID.c_str();
		else
			stageToLoad = Configuration::worldData.data[lastValidFlagSelected].data[stageSelectedWindow].levelID.c_str();
	}
	return stageToLoad;
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
void SetVisibilityCapitalWindow(bool visible)
{
	selectedCapital = 0;
	if (visible) //im only doing this so that it doesnt disable when disabling and plays anims instead
	{
		cts_choices_bg->SetHideFlag(!visible);
		cts_choices_window->SetHideFlag(!visible);
	}
	cts_choices_select->SetHideFlag(!visible);
	CSDCommon::PlayAnimation(*cts_choices_bg, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, true, !visible);
	CSDCommon::PlayAnimation(*cts_choices_window, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, true, !visible);

	deco_text[6]->SetHideFlag(!visible);
	deco_text[7]->SetHideFlag(!visible);

	CSDCommon::PlayAnimation(*deco_text[6], "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*deco_text[7], "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*cts_choices_select, "Scroll_down_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 0, 100);
}
void SetVisibilityStageWindow(bool visible)
{
	CSDCommon::PlayAnimation(*cts_stage_window, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, true, !visible);
	CSDCommon::PlayAnimation(*cts_name_2, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, true, !visible);
	CSDCommon::PlayAnimation(*cts_stage_select, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, true, !visible);
	CSDCommon::PlayAnimation(*stageSelectFlag, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, true, !visible);
	CSDCommon::PlayAnimation(*cts_stage_ss, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, true, !visible);
	CSDCommon::PlayAnimation(*cts_stage_info_window, "Intro_Anim_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, true, !visible);
	CSDCommon::FreezeMotion(*cts_stage_select, 0);

	PopulateStageSelect(lastValidFlagSelected);
	cts_stage_select->SetHideFlag(!visible);
	cts_stage_5_medal->SetHideFlag(!visible);
	cts_guide_3_rank->SetHideFlag(!visible);
	cts_guide_1_hiscore->SetHideFlag(!visible);
	cts_guide_2_besttime->SetHideFlag(!visible);
	if (visible)  //im only doing this so that it doesnt disable when disabling and plays anims instead
	{

		CSDCommon::PlayAnimation(*cts_guide_txt, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0);
		SetLevelTextCast("Head for the goal!");
		cts_guide_txt->SetPosition(0, 40);
		cts_stage_window->SetHideFlag(false);
		cts_stage_ss->SetHideFlag(false);
		cts_name_2->SetHideFlag(false);
		stageSelectFlag->SetHideFlag(false);
		cts_stage_info_window->SetHideFlag(false);
	}	
	else

		cts_guide_txt->SetPosition(140, 40);
}
void TitleWorldMap::CreateScreen(Sonic::CGameObject* pParentGameObject)
{
	if (rcWorldMap && !spWorldMap)
	{
		pParentGameObject->m_pMember->m_pGameDocument->AddGameObject(spWorldMap = boost::make_shared<Sonic::CGameObjectCSD>(rcWorldMap, 0.5f, "HUD", false), "main", pParentGameObject);
	}
}
void TitleWorldMap::KillScreen()
{
	if (spWorldMap)
	{
		spWorldMap->SendMessage(spWorldMap->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
		spWorldMap = nullptr;
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
	Eigen::Vector3f* lightColor = (Eigen::Vector3f*)0x01A42308;
	lightColor->x() = 0.5976471f;
	lightColor->y() = 0.5835295f;
	lightColor->z() = 0.5364707f;
	if (light == nullptr)
		light = Sonic::CGameDocument::GetInstance()->m_pMember->m_spLightManager;
	light->m_GlobalLightDirection = CVector(1,1,1);

	//Set light properties
	light->m_GlobalLightDiffuse = CVector(0.02f, 0.02f, 0.02f);
	//light->m_GlobalLightDirection = CVector(-79.8565f, 0, 4.78983f);
	light->m_GlobalLightSpecular = CVector(1, 1, 1);
	Sonic::CCsdDatabaseWrapper wrapper(This->m_pMember->m_pGameDocument->m_pMember->m_spDatabase.get());
	auto spCsdProject = wrapper.GetCsdProject("ui_worldmap");
	rcWorldMap = spCsdProject->m_rcProject;

	//auto e = DecorationTextManager::InitializeText("const char* text", 1, 0, Hedgehog::math::CVector(0, 0, 0));
	infobg1 = rcWorldMap->CreateScene("info_bg_1");
	infoimg1 = rcWorldMap->CreateScene("info_img_1");
	infoimg2 = rcWorldMap->CreateScene("info_img_2");
	infoimg3 = rcWorldMap->CreateScene("info_img_3");
	infoimg4 = rcWorldMap->CreateScene("info_img_4");

	headerBGW = rcWorldMap->CreateScene("worldmap_header_bg");
	footerBGW = rcWorldMap->CreateScene("worldmap_footer_bg");
	footerIMGW = rcWorldMap->CreateScene("worldmap_footer_img");
	headerIMGW = rcWorldMap->CreateScene("worldmap_header_img");
	worldmap_footer_img_A = rcWorldMap->CreateScene("worldmap_footer_img_A");
	cts_guide_window = rcWorldMap->CreateScene("cts_guide_window");
	cts_guide_ss = rcWorldMap->CreateScene("cts_guide_ss");
	cts_stage_ss = rcWorldMap->CreateScene("cts_guide_ss");
	cts_guide_txt = rcWorldMap->CreateScene("cts_guide_txt");
	cts_name = rcWorldMap->CreateScene("cts_name");
	cts_cursor_effect = rcWorldMap->CreateScene("cts_cursor_effect");
	cts_guide_4_misson = rcWorldMap->CreateScene("cts_guide_4_misson");
	cts_guide_5_medal = rcWorldMap->CreateScene("cts_guide_5_medal");

	cts_guide_1_hiscore = rcWorldMap->CreateScene("cts_guide_1_hiscore");
	cts_guide_2_besttime = rcWorldMap->CreateScene("cts_guide_2_besttime");
	cts_guide_3_rank = rcWorldMap->CreateScene("cts_guide_3_rank");
	cts_stage_4_misson = rcWorldMap->CreateScene("cts_guide_4_misson");
	cts_stage_5_medal = rcWorldMap->CreateScene("cts_guide_5_medal");


	cts_stage_window = rcWorldMap->CreateScene("cts_stage_window");
	cts_stage_info_window = rcWorldMap->CreateScene("cts_guide_window");
	cts_stage_select = rcWorldMap->CreateScene("cts_stage_select");
	cts_name_2 = rcWorldMap->CreateScene("cts_name_2");
	stageSelectFlag = rcWorldMap->CreateScene("cts_stage_flag");

	cts_choices_bg = rcWorldMap->CreateScene("cts_choices_bg");
	cts_choices_window = rcWorldMap->CreateScene("cts_choices_window");
	cts_choices_select = rcWorldMap->CreateScene("cts_choices_select");
	CSDCommon::PlayAnimation(*infobg1, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*infoimg1, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*infoimg2, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*infoimg3, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*infoimg4, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*headerBGW, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*footerBGW, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*footerBGW, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*headerIMGW, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*worldmap_footer_img_A, "Intro_Anim_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*cts_cursor_effect, "Select_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	CSDCommon::PlayAnimation(*cts_name, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);

	
	for (size_t i = 0; i < 6; i++)
	{
		//Set text nodes to the exact positions they should be at according to the animation
		deco_text[i] = rcWorldMap->CreateScene("deco_text");
		CSDCommon::PlayAnimation(*deco_text[i], "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		
			CSDCommon::PlayAnimation(*cts_stage_select, "Select_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, (i) * 10, i * 10);
			deco_text[i]->SetPosition(cts_stage_select->GetNode("select_img")->GetPosition().x(), cts_stage_select->GetNode("select_img")->GetPosition().y());
		


		//Set text
		char actname[6];
		sprintf(actname, "Act %d", i + 1);
		deco_text[i]->GetNode("Text_blue")->SetText(actname);
	}
	deco_text[6] = rcWorldMap->CreateScene("deco_text_centered");
	deco_text[7] = rcWorldMap->CreateScene("deco_text_centered");

	deco_text[6]->GetNode("Text_blue")->SetText("Go to capital");
	deco_text[7]->GetNode("Text_blue")->SetText("Select Stage");

	deco_text[6]->SetPosition(LetterboxHelper::ScreenHalfPoint->x(), LetterboxHelper::ScreenHalfPoint->y() - 45);
	deco_text[7]->SetPosition(LetterboxHelper::ScreenHalfPoint->x(), LetterboxHelper::ScreenHalfPoint->y() + 25);
	ShowTextAct(false);
	deco_text[6]->SetHideFlag(true);
	deco_text[7]->SetHideFlag(true);


	//By default the cursor in the worldmap is set to the left anchor 
	cursorL = rcWorldMap->CreateScene("cts_cursor");
	cursorT = rcWorldMap->CreateScene("cts_cursor");
	cursorB = rcWorldMap->CreateScene("cts_cursor");
	cursorR = rcWorldMap->CreateScene("cts_cursor");
	camInitialized = false;
	constexpr float earthRadius = 5.25f; //Used to normalize flag positions to the globe's curvature
	flagPositions.push_back(CVector(0.31f, 0.36f, 2.28f));
	flagPositions.push_back(CVector(2.310000f, 2.360000f, 1.111371f));
	flagPositions.push_back(CVector(2.810000f, -0.140000f, -6.649425f));
	flagPositions.push_back(CVector(2.810000f, -1.890000, 1.742745));
	flagPositions.push_back(CVector(-0.190000, 4.610000, -3.543527));
	flagPositions.push_back(CVector(-5.190000, 0.110000, -3.363136));
	flagPositions.push_back(CVector(0.060000, -2.639999, -6.829812));
	flagPositions.push_back(CVector(-4.440000, -2.390000, -0.798426));
	flagPositions.push_back(CVector(-3.600000, 3.00000, -1.160)); 

	// Now normalize all these positions
	for (int i = 0; i < flagPositions.size(); ++i)
	{
		flagPositions.at(i) = ((flagPositions.at(i) - TitleWorldMap::emblemPosition).normalized() * earthRadius) + TitleWorldMap::emblemPosition;
	}
	//Set up sun/moon medals' animations
	for (size_t i = 0; i < 9; i++)
	{
		flag[i] = FlagUIInformation();
		
		flag[i].flag = rcWorldMap->CreateScene("cts_parts_flag");
		flag[i].sun_moon = rcWorldMap->CreateScene("cts_parts_sun_moon");

		flag[i].playingMedalTransition = false;
		CSDCommon::PlayAnimation(*flag[i].flag, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		flag[i].flag->GetNode("img")->SetPatternIndex(i);

		float shadeAmount = fmax(0.0f, -(CVector(-79.8565f, 0, 4.78983f).dot((flagPositions.at(i) - CVector(0, 0, -2.34f)).normalized()))) * 100;
		float visibility = fmax(0.0f, -(CVector(0, 0, -20).dot((flagPositions.at(i) - CVector(0, 0, -2.34f)).normalized()))) * 100;
		bool isDark = shadeAmount > 50;

		isInShade[i] = isDark;
		CSDCommon::PlayAnimation(*flag[i].sun_moon, "Switch_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 0, isDark ? 0 : 100); //Set image to either sun or moon
		CSDCommon::PlayAnimation(*flag[i].sun_moon, "Fade_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 0, visibility, visibility);
	}
	TitleWorldMap::CreateScreen(This);
	for (auto e : flag)	e.flag->SetHideFlag(true);
	
	SetHideEverythingWM(true);
	cts_guide_4_misson->SetHideFlag(true);
	cts_guide_5_medal->SetHideFlag(true);
	cts_stage_window->SetHideFlag(true);
	cts_stage_select->SetHideFlag(true);
	cts_name_2->SetHideFlag(true);
	cts_cursor_effect->SetHideFlag(true);
	stageSelectFlag->SetHideFlag(true);
	cts_stage_info_window->SetHideFlag(true);
	cts_stage_4_misson->SetHideFlag(true);
	cts_stage_5_medal->SetHideFlag(true);
	cts_guide_1_hiscore->SetHideFlag(true);
	cts_guide_2_besttime->SetHideFlag(true);
	cts_guide_3_rank->SetHideFlag(true);
	cts_choices_bg->SetHideFlag(true);
	cts_choices_window->SetHideFlag(true);
	cts_choices_select->SetHideFlag(true);
	cts_stage_ss->SetHideFlag(true);

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

//void GetRings(Sonic::CGameObject* This, void* Edx)
//{
//	float* gg = 0;
//	auto e = sub_A51F90(sub_A515F0(), 9, gg, Edx);
//	printf("%x", e);
//}

void Flags_Update() 
{
	for (size_t i = 0; i < 9; i++)
	{
		float visibility = fmax(0.0f, -(TitleWorldMap::Camera->m_MyCamera.m_Direction.dot((flagPositions.at(i) - CVector(0, 0, -2.34f)).normalized()))) * 100;
		auto uiPos = WorldToUIPosition(flagPositions.at(i));

		float shadeAmount = fmax(0.0f, -(light->m_GlobalLightDirection.dot((flagPositions.at(i) - CVector(0, 0, -2.34f)).normalized()))) * 100;
		bool isDark = shadeAmount > 50;
		if (flag[i].playingMedalTransition && flag[i].sun_moon->m_MotionFrame == 0 || flag[i].sun_moon->m_MotionFrame == flag[i].sun_moon->m_MotionEndFrame) flag[i].playingMedalTransition = false;

		if (introPlayed)
			CSDCommon::PlayAnimation(*flag[i].flag, "Fade_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 0, visibility, visibility);

		if (!flag[i].playingMedalTransition)
			CSDCommon::PlayAnimation(*flag[i].sun_moon, "Fade_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 0, visibility, visibility);

		flag[i].flag->SetPosition(uiPos.x(), uiPos.y());
		flag[i].sun_moon->SetPosition(uiPos.x() + 36, uiPos.y() - 23);
		flag[i].night = !isDark;

		if (isDark && !isInShade[i])
		{
			flag[i].playingMedalTransition = true;
			CSDCommon::PlayAnimation(*flag[i].sun_moon, "Switch_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 0, 0, 0, true, true);
		}
		else if (!isDark && isInShade[i])
		{
			flag[i].playingMedalTransition = true;
			CSDCommon::PlayAnimation(*flag[i].sun_moon, "Switch_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 0, 0);
		}

		bool inrange = IsInsideCursorRange(uiPos, visibility, i);
		currentFlagSelected = inrange ? i : -1;
		if (currentFlagSelected != -1)
		{
			lastValidFlagSelected = currentFlagSelected;
			if (isDark && !isInShade[i])
				MiniAudioHelper::PlaySound(cursorSelectHandle, 15, "Sunset");
			else if (!isDark && isInShade[i])
				MiniAudioHelper::PlaySound(cursorSelectHandle, 16, "Sunrise");
		}
		isInShade[i] = isDark;
		flagSelectionAmount += inrange;
	}
}
void CapitalWindow_Update()
{
	if (timeStageSelectDelay < 5)
		timeStageSelectDelay += 1;
	auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->m_CurrentPadStateIndex];
	//Selection increase
	if (inputPtr->IsTapped(Sonic::eKeyState_LeftStickDown) && selectedCapital == 0)
	{
		selectedCapital += 1;
		MiniAudioHelper::PlaySound(stageSelectHandle, 0, "Cursor", false);
			CSDCommon::PlayAnimation(*cts_choices_select, "Scroll_up_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);

	}
	//Selection decrease
	if (inputPtr->IsTapped(Sonic::eKeyState_LeftStickUp) && selectedCapital == 1)
	{
		selectedCapital -= 1;
		MiniAudioHelper::PlaySound(stageSelectHandle, 0, "Cursor", false);
			CSDCommon::PlayAnimation(*cts_choices_select, "Scroll_down_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1,0);

	}
	Common::ClampInt(selectedCapital, 0, 1);

	if (inputPtr->IsTapped(Sonic::eKeyState_A) && timeStageSelectDelay >= 5)
	{
		if (selectedCapital == 0)
		{
			MiniAudioHelper::PlaySound(stageSelectHandle, 3, "Boot");
			isWhiteWorldEnabled = Configuration::worldData.data[lastValidFlagSelected].data[Configuration::GetCapital(lastValidFlagSelected)].isWhiteWorld;

			/*if (isWhiteWorldEnabled)
			{
				WRITE_STRING(0x015B499C, GetStageToLoad());
				WRITE_STRING(0x015C789C, GetStageToLoad());
				WRITE_STRING(0x015C8148, GetStageToLoad());
				WRITE_STRING(0x0169A304, GetStageToLoad());

			}*/
			Title::canLoad = 1;
		}
		else
		{
			MiniAudioHelper::PlaySound(stageSelectHandle, 2, "Accept");
			SetVisibilityStageWindow(true);
			SetVisibilityCapitalWindow(false);
			//ShowTextAct(true);
			capitalWindowOpen = false;
			timeStageSelectDelay = 0;

		}
	}
}
void SetStageSelectionScreenshot()
{
	if (!cts_stage_ss)
		return;
	char nameCast[16];
	for (size_t i = 0; i < 9; i++)
	{
		sprintf(nameCast, "ss_%02dd", i + 1);
		cts_stage_ss->GetNode(nameCast)->SetHideFlag(true);
	}
	sprintf(nameCast, "ss_%02dd", lastValidFlagSelected+1);

	cts_stage_ss->GetNode(nameCast)->SetHideFlag(false);
}
///Stage selection highlight & stage launch
void StageWindow_Update(Sonic::CGameObject* This) 
{
	auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->m_CurrentPadStateIndex];
	//Pressing A requires a delay because otherwise even just tapping it will press A twice when this is called and it'll launch the stage immediately
	if (timeStageSelectDelay < 5)
		timeStageSelectDelay += 1;
	//Enables loading to the stage
	if (inputPtr->IsTapped(Sonic::eKeyState_A) && timeStageSelectDelay >= 5)
	{
		MiniAudioHelper::PlaySound(stageSelectHandle, 3, "Boot");

		isWhiteWorldEnabled = Configuration::worldData.data[lastValidFlagSelected].data[stageSelectedWindow].isWhiteWorld;
		Title::canLoad = 1;

		
	}
	//Selection increase
	if (inputPtr->IsTapped(Sonic::eKeyState_LeftStickDown) && stageSelectedWindow != stageSelectedWindowMax)
	{
		stageSelectedWindow += 1;

		MiniAudioHelper::PlaySound(stageSelectHandle, 0, "Cursor", false);
		if (stageSelectedWindow > 0) //technically not needed
			CSDCommon::PlayAnimation(*cts_stage_select, "Select_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, (stageSelectedWindow - 1) * 10, stageSelectedWindow * 10);
	}
	//Selection decrease
	if (inputPtr->IsTapped(Sonic::eKeyState_LeftStickUp))
	{
		stageSelectedWindow -= 1;
		MiniAudioHelper::PlaySound(stageSelectHandle, 0, "Cursor", false);
		if (stageSelectedWindow < 5)
			CSDCommon::PlayAnimation(*cts_stage_select, "Select_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 130 - ((stageSelectedWindow + 1) * 10), 130 - (stageSelectedWindow) * 10);

	}
	SetStageSelectionScreenshot();

	Common::ClampInt(stageSelectedWindow, 0, stageSelectedWindowMax);

	PopulateStageWindowInfo(Configuration::worldData.data[lastValidFlagSelected].data[stageSelectedWindow].levelID);
}
HOOK(void*, __fastcall, TitleW_UpdateApplication, 0xE7BED0, Sonic::CGameObject* This, void* Edx, float elapsedTime, uint8_t a3)
{
	auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->m_CurrentPadStateIndex];
	float multiplier = 24;
	uint32_t stageTerrainAddress = Common::GetMultiLevelAddress(0x1E66B34, { 0x4, 0x1B4, 0x80, 0x20 });
	char** h = (char**)stageTerrainAddress;
	const char* terr = *h; 
	
	if (TitleWorldMap::Active)
	{
		lastStageID = terr;
		//Pretty much just "if titleworldmap exists"
		if (flag[0].flag && cts_guide_4_misson && cts_guide_5_medal)
		{
			if (infoimg2->m_MotionDisableFlag) CSDCommon::PlayAnimation(*infoimg2, "Usual_Anim", Chao::CSD::eMotionRepeatType_Loop, 0, 0);
			if (infoimg3->m_MotionDisableFlag)CSDCommon::PlayAnimation(*infoimg3, "Usual_Anim", Chao::CSD::eMotionRepeatType_Loop, 0, 0);
			if (infoimg4->m_MotionDisableFlag)CSDCommon::PlayAnimation(*infoimg4, "Usual_Anim", Chao::CSD::eMotionRepeatType_Loop, 0, 0);

			multiplierRotationLight = inputPtr->IsDown(Sonic::eKeyState_X) ? 10 : 0.005;
			if (!stageWindowOpen)
				SetCursorPos(CVector2(inputPtr->LeftStickHorizontal * multiplier, -inputPtr->LeftStickVertical * multiplier));

			//GetRings(This, Edx);

			flagSelectionAmount = 0;
			if (TitleWorldMap::Camera)
			{
				if (flag[0].flag->m_MotionDisableFlag && !introPlayed)
				{
					introPlayed = true;
				}

				Flags_Update();

				if (flagSelectionAmount != lastflagSelectionAmount && currentFlagSelected == lastFlagIn)
				{
					if (flagSelectionAmount > 0)
					{
						//Show country info window, etc
						cursorSelected = true;
						CSDCommon::PlayAnimation(*cts_name, "Intro_1_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
						CSDCommon::PlayAnimation(*cts_cursor_effect, "Select_Anim", Chao::CSD::eMotionRepeatType_Loop, 1, 0);
						PlayCursorAnim("Select_Anim");
						cts_guide_4_misson->SetHideFlag(false);
						cts_guide_5_medal->SetHideFlag(false);
						CSDCommon::FreezeMotion(*cts_guide_ss, 0);
						CSDCommon::FreezeMotion(*cts_guide_txt, 0);
						CSDCommon::FreezeMotion(*cts_guide_window, 0);
						CSDCommon::PlayAnimation(*cts_guide_ss, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
						CSDCommon::PlayAnimation(*cts_guide_txt, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
						CSDCommon::PlayAnimation(*cts_guide_window, "Intro_Anim_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);

						MiniAudioHelper::PlaySound(cursorSelectHandle, 800004, "CursorSelect", false);
						/*Common::MiniAudioHelper::PlaySoundStatic(cursorSelectHandle, 800004);
						Common::MiniAudioHelper::PlaySoundStatic(cursorSelectHandle, 17);*/

						PopulateCountryPreviewInfo(lastValidFlagSelected);
						SetLevelTextCast(Configuration::worldData.data[lastValidFlagSelected].description.c_str());
					}
					else
					{
						//Hide country info
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
				if (flagSelectionAmount > 0 && introPlayed)
				{
					//Open Stage Selection
					if (inputPtr->IsTapped(Sonic::eKeyState_A) && !stageWindowOpen)
					{
						stageWindowOpen = true;
						stageSelectedWindow = 0;
						PopulateStageSelect(lastValidFlagSelected);
						SetVisibilityPlayerInfo(false);

						MiniAudioHelper::PlaySound(stageSelectHandle, 2, "OpenWindow", false);
						cts_name_2->GetNode("img_1")->SetPatternIndex(lastValidFlagSelected);
						stageSelectFlag->GetNode("img")->SetPatternIndex(lastValidFlagSelected);

						if (Configuration::GetCapital(lastValidFlagSelected) != -1)
						{
							capitalWindowOpen = true;
							ShowTextAct(false);
							SetVisibilityCapitalWindow(true);
						}
						else
						{
							SetVisibilityStageWindow(true);
						}
					}
					if (inputPtr->IsTapped(Sonic::eKeyState_B) && stageWindowOpen)
					{
						stageWindowOpen = false;

						SetVisibilityPlayerInfo(true);

						timeStageSelectDelay = 0;
						MiniAudioHelper::PlaySound(stageSelectHandle, 4, "Cancel", false);

						if (capitalWindowOpen)
							SetVisibilityCapitalWindow(false);
						else
							SetVisibilityStageWindow(false);
						ShowTextAct(false);
						capitalWindowOpen = false;
					}
					if (stageWindowOpen && !capitalWindowOpen)
					{
						StageWindow_Update(This);
					}
					if (capitalWindowOpen)
					{
						CapitalWindow_Update();
					}
				}
				cts_name->SetHideFlag(!cursorSelected);
				cts_cursor_effect->SetHideFlag(!cursorSelected);
				lastflagSelectionAmount = flagSelectionAmount;
			}
			CheckCursorAnimStatus();
		}
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
float LerpEaseInOut(float start, float end, float time, bool inEnabled, bool outEnabled)
{
	Common::ClampFloat(time, 0, 1);
	time = (time < 0.5f) ? (inEnabled ? (2.0f * time * time) : time) : (outEnabled ? (-1.0f + (4.0f - 2.0f * time) * time) : time);  // Ease in/out
	/*if (start <= end)
	{*/
		return start + (end - start) * time;  // Interpolate and return result
	//}
	//else
	//{
	//	return end + (start - end) * time;  // Interpolate and return result
	//}
}

void PlayPan(CustomCamera* camera, const Hedgehog::Universe::SUpdateInfo& updateInfo)
{
	if (timePan >= 2.5f)
	{
		playingPan = false;
		disabledTarget = false;
	}
	timePan += updateInfo.DeltaTime;
	camHeight = LerpEaseInOut(-20, 0, timePan / 2.15f, true, false);
	rotationPitch = LerpEaseInOut(-0.4f, -0.5f, timePan / 2.15f, true, true);
	//FOV = LerpEaseInOut(0.84906584f, 0.44906584f, timePan / 2.15f, false, true);

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
	if (playingPan || disabledTarget || !TitleWorldMap::Active)
		return;
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
float s_RotationAngle;
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
	
	if (!camInitialized)
	{
		camInitialized = true;
		camera->m_Position = CVector(0, 0, cameraDistance);
		camera->m_TargetPosition = CVector(0, 0, 0);
		light = Sonic::CGameDocument::GetInstance()->m_pMember->m_spLightManager;
	}
	// Some stuff that'll help us in the future.

	CVector cameraTargetPosition = CVector(0, camHeight, 0);
	const CVector cameraVector = CVector(0, 0, cameraDistance);
	const CVector cameraPosition = cameraVector + cameraTargetPosition;

	// We can make this a parameter or something later.
	constexpr float rotationPitchRate = 1.5f;
	constexpr float rotationYawRate = 1.5f;

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
		for (CVector position : flagPositions)
		{
			const CVector direction = (position - TitleWorldMap::emblemPosition).normalized();
			if (-direction.dot(camera->m_MyCamera.m_Direction) < dotThreshold && currentFlagSelected == -1)
				continue;
			if (introPlayed)
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

	camera->m_FieldOfView = FOV;
	camera->m_FieldOfViewTarget = FOV;


	constexpr float rotationhRateLight = 0.1f;

		

	const CVector lightPosition = CVector(-79.8565f, 0, 4.78983f);
	//Set up light stuff
	const float rotationForce = rotationhRateLight * multiplierRotationLight * updateInfo.DeltaTime;
	s_RotationAngle += rotationForce;
	// Wrap this around 360 degrees if you feel so inclined
	s_RotationAngle = WrapAroundFloat(s_RotationAngle, 360.0 * DEG2RAD);
	const CQuaternion lightRotation = TitleWorldMap::QuaternionFromAngleAxis(-s_RotationAngle, CVector(0, 1, 0));
	light->m_GlobalLightDirection = lightRotation * (lightPosition - TitleWorldMap::emblemPosition) + TitleWorldMap::emblemPosition;

	//Set light properties
	light->m_GlobalLightDiffuse = CVector(0.02f, 0.02f, 0.02f);
	//light->m_GlobalLightDirection = CVector(-79.8565f, 0, 4.78983f);
	light->m_GlobalLightSpecular = CVector(1, 1, 1);


	ApplyCameraStuff(This, camera);
	camera->UpdateParallel(updateInfo);


}
#pragma endregion

FUNCTION_PTR(void, __thiscall, Hedgehog_Base_CHolderBase___ct, 0x0065FBE0, Hedgehog::Base::CHolderBase* This, void* synchronizedObject, char forceSync);
void GetCServiceGameplay(void* arg1, void* serviceRetrn, DWORD* a3)
{
	uint32_t func = 0x40F1C0;
	uint32_t Hedgehog__Base__CHolderBase____ct = 0x0065FBE0;
	uint32_t stuff = 0x1E66B34;
	__asm
	{
		mov     esi, arg1
		lea     edx, a3
		push    edx
		lea     eax, serviceRetrn
		add     esi, 34h
		push    eax
		mov     ecx, esi
		mov[esp + 58h + a3], ebx
		call    func
	};
};
char* stagePrev;
Hedgehog::Universe::CStateMachineBase* GameplayFlow;
void SetCorrectStageFromFlag()
{
	camInitialized = false;
	if (!TitleWorldMap::LoadingReplacementEnabled)
	{
		if (Title::InInstall)
		{
			uint32_t stageTerrainAddress = Common::GetMultiLevelAddress(0x1E66B34, { 0x4, 0x1B4, 0x80, 0x20 });
			char** h = (char**)stageTerrainAddress;
			const char* terr = *h;
			
			if (terr != lastStageID && lastStageID[0] != '\0')
				strcpy(*(char**)stageTerrainAddress, lastStageID);
		}
		return;
	}
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
	{
		stageToLoad = GetStageToLoad();
	}
	Hedgehog::Base::CHolderBase v8;
	void* serviceRetrn;
	DWORD a3;
	Hedgehog_Base_CHolderBase___ct(&v8, (Hedgehog::Base::TSynchronizedPtr<Sonic::CApplicationDocument>*)0x1E66B34, 0);
	//GetCServiceGameplay(v8.get(), &serviceRetrn, &a3);
	DebugDrawText::log((std::string("Stage Loading: ") + std::string(stageToLoad)).c_str());
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

void __declspec(naked) InterceptGameplayFlowLoading()
{
	static uint32_t normal = 0x00D0E166;
	static uint32_t loc_D0E170 = 0xD0E170;
	__asm
	{ 
		cmp TitleWorldMap::LoadingReplacementEnabled, 1
		jne FunctionFinish
		//Original function
		mov     esi, eax
		xor eax, eax
		cmp TitleWorldMap::ForceLoadToFlowTitle, 1
		je ChangeToTitle

		cmp ecx, 5
		//Change module only if its white world
		je ChangeToAct
		jmp[loc_D0E170]

		ChangeToAct :
		cmp isWhiteWorldEnabled, 1
		je FunctionFinish
		mov ecx, 6
		jmp[loc_D0E170]

		ChangeToTitle :
		mov ecx, 3
		jmp[loc_D0E170]

		FunctionFinish:
		jmp[loc_D0E170]
	}
}
void __declspec(naked) SetCorrectPause()
{
	static uint32_t normal = 0x010A0B4A;
	__asm
	{ 
		sub     esp, 38h
		push    esi
		cmp isWhiteWorldEnabled, 1
		je SetCorrectValue
		jmp[normal]
		
		SetCorrectValue:
		mov eax, 2
		jmp[normal]
	}
}

HOOK(void, __fastcall, CGameplayFlowStageCStateGoalBegin, 0xCFD550, void* This)
{
	//TitleWorldMap::ForceLoadToFlowTitle = true;
	originalCGameplayFlowStageCStateGoalBegin(This);
}
HOOK(int, __fastcall, StageLoader, 0x00D027D0, int* This)
{
	if (*(This + 400))
	{
		printf("yes");
	}
	return originalStageLoader(This);
}
HOOK(int, __fastcall, UVAnimStart, 0x007597E0, Sonic::CGameObject* This, void* Edx, int a2, Hedgehog::Base::CSharedString* modelName, int flag)
{

	printf("\nLoaded UV-Anim: %s", modelName->c_str());
	return originalUVAnimStart(This, Edx, a2,modelName, flag );
}
FUNCTION_PTR(int, __thiscall, PlayUVAnimMaybe, 0x00752F00, Hedgehog::Mirage::CRenderable* This, float a2);

HOOK(Hedgehog::Mirage::CRenderable*, __fastcall, AddRenderableTitle, 0x58E650, DWORD* This, int a2, int a3, int a4, void* Edx)
{
	auto e = originalAddRenderableTitle(This, a2, a3, a4, Edx);
	//PlayUVAnimMaybe(e, 0.0f);
	return e;
}
HOOK(char, __stdcall, SaveRead, 0x00E7A220, int a1, void* file)
{
	return originalSaveRead(a1,file);
}
void TitleWorldMap::Install()
{
	//Disable Title music
	//WRITE_JUMP(0x00571EC9, 0x00571ED5);
	//WRITE_JUMP(0x005727DB, 0x005727E7);
	WRITE_JUMP(0xD56CCA, SetCorrectTerrainForMission_ASM);
	WRITE_JUMP(0x00D0E164, InterceptGameplayFlowLoading);
	
	//WRITE_JUMP(0x00D9565B, SetCorrectPlayer2);
	//WRITE_JUMP(0x00D961E2, 0x00D961E6);
	//WRITE_JUMP(0x00D76A45, SetCorrectPlayer2);
	//WRITE_JUMP(0x00D0E164, StageLoader);
	//WRITE_JUMP(0x010A0B46, SetCorrectPause);
	INSTALL_HOOK(StageLoader);
	INSTALL_HOOK(AddRenderableTitle);
	WRITE_JUMP(0x00584CEE, (void*)0x00588820);
	WRITE_JUMP(0x015B8188, (void*)0x015B8198);
	WRITE_JUMP(0x0058D41F, (void*)0x0058D7D8);//Skip setting light properties every second
	
	WRITE_JUMP(0x00D0A3F0, (void*)0x00D0A4C1); //Ignore PAM position & rotation
	INSTALL_HOOK(UVAnimStart);
	INSTALL_HOOK(CGameplayFlowStageCStateGoalBegin);
	INSTALL_HOOK(SaveRead);
	INSTALL_HOOK(Title_CameraUpdate);
	INSTALL_HOOK(TitleW_CMain);
	INSTALL_HOOK(TitleW_UpdateApplication);
	WRITE_MEMORY(0x016E11F4, void*, CTitleWRemoveCallback);

	//From Brianuu's 06 Title, makes saving not crash
	WRITE_JUMP(0xD22A83, (void*)0xD22B84);
	WRITE_MEMORY(0xD22CE8, uint8_t, 0);

	//WRITE_JUMP(0x00A51B66, 0x00A51BCE);
}
