#pragma once

class Title
{
public:
	static void Install();
	static void CreateScreen(Sonic::CGameObject* pParentGameObject);
	static void ToggleScreen(const bool visible, Sonic::CGameObject* pParentGameObject);
	static void KillScreen();
	static void FreezeMotion(Chao::CSD::CScene* pScene);
	static void PlayAnimation(Chao::CSD::CScene* pScene, const char* name, Chao::CSD::EMotionRepeatType repeatType, float motionSpeed, float startFrame);
	static void IntroAnim(Chao::CSD::RCPtr<Chao::CSD::CScene> scene);
};
