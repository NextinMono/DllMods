#pragma once

class HudSonicStage
{


public:
	static void Install();
	static void CreateScreen(Sonic::CGameObject* pParentGameObject);
	static void ToggleScreen(const bool visible, Sonic::CGameObject* pParentGameObject);
	static void KillScreen();

	static int redRingCount;
	static int prevRedRingCount;
	static enum StageAnimMode 
	{
		Idle,
		IdleWisp,
		IntroWait,
		Intro,
		Usual,
		BoostingNormal,
		WispIdle,
		BoostingWisp,
		WispShake,
		Outro
	};

};
