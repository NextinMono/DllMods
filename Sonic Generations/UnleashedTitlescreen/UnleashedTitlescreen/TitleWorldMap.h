#pragma once
class CustomCamera : public Sonic::CGameObject, public Hedgehog::Universe::TStateMachine<CustomCamera>
{
public:
	class CMyCamera : public Hedgehog::Mirage::CCamera
	{
	public:
		Hedgehog::Math::CMatrix m_InputView;
	} m_MyCamera;

	BB_INSERT_PADDING(0x10);
	Hedgehog::Math::CVector m_Position;
	Hedgehog::Math::CVector m_TargetPosition;
	Hedgehog::Math::CVector m_UpVector;
	BB_INSERT_PADDING(0x1DC);
	uint32_t m_ActorID;
	BB_INSERT_PADDING(0x08);
	float m_FieldOfView;
	float m_FieldOfViewTarget;
	BB_INSERT_PADDING(0x50);
};

class TitleWorldMap
{

public:

	// Earth's position is in the center of the screen.
	// Later, we'll want to be able to move both the camera target & position in synch so the camera can smoothly move downward.
	static const Hedgehog::Math::CVector emblemPosition;
	static const CustomCamera* Camera;
	static void Install();
	static void Start();
	static void CreateScreen(Sonic::CGameObject* pParentGameObject);
	static void ToggleScreen(const bool visible, Sonic::CGameObject* pParentGameObject);
	static void KillScreen();
	static void SetHideEverything(const bool visible);
	static void IntroAnim(Chao::CSD::RCPtr<Chao::CSD::CScene> scene);
	static hh::math::CQuaternion QuaternionFromAngleAxis(float angle, const hh::math::CVector& axis);
	static void PlayPanningAnim();
	static void EnableInput();

	struct SaveStageInfo 
	{
		std::string stageID_string;
		uint32_t stageIDForGens;
		bool isStageCompleted;
		uint32_t bestScore;
		float bestTime;
		float bestTime2;
		float bestTime3;
		uint32_t bestRank;
		uint32_t bestRing;
		uint32_t redRingCount;
	};
	/*struct RGBColor
	{
		float r;
		float g;
		float b;

		RGBColor operator*(const float& f)
		{
			return RGBColor(r * f, g * f, b * f);
		}
	};*/
};
