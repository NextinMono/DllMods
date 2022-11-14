#pragma once
#include "Camera.h"
class Title
{
	
public:
	Camera* camera;

	enum TitleState {
		WaitForMovie,
		ExecSubmenu,

	};
	static void Install();
	static void CreateScreen(Sonic::CGameObject* pParentGameObject);
	static void ToggleScreen(const bool visible, Sonic::CGameObject* pParentGameObject);
	static void KillScreen();
	static void SetHideEverything(const bool visible);
	static void SetSubmenu(bool enabled);
	static void IntroAnim(Chao::CSD::RCPtr<Chao::CSD::CScene> scene);
};
