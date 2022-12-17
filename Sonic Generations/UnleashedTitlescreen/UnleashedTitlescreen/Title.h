#pragma once
class Title
{
	
public:

	enum TitleState {
		WaitForMovie,
		ExecSubmenu,

	};

	static enum TitleIndexState {
		New_Game,
		Continue,
		Options,
		Quit
	};
	static void Install();
	static void CreateScreen(Sonic::CGameObject* pParentGameObject);
	static void ToggleScreen(const bool visible, Sonic::CGameObject* pParentGameObject);
	static void KillScreen();
	static void SetHideEverything(const bool visible);
	static void SetSubmenu(bool enabled);
	static void SetScrollDirection(bool horizontal);
	static void IntroAnim(Chao::CSD::RCPtr<Chao::CSD::CScene> scene);
};
