#pragma once
struct SaveLoadTestStruct
{
	INSERT_PADDING(0x34);
	bool m_saveCompleted;
};
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
	static void SetHideEverything(const bool visible, const bool logoVisible = false);
	static void SetSubmenu(bool enabled);
	static void SetScrollDirection(bool horizontal);
	static void IntroAnim(Chao::CSD::RCPtr<Chao::CSD::CScene> scene);
	static uint32_t* OutState;
	static SharedPtrTypeless MusicHandle;
	static bool canLoad;
	static bool InInstall;
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
// Custom class definition so we have fields mapped out that BlueBlur does not yet.
// In the future, you will want to replace all references to "CustomCamera" with "Sonic::CCamera" when those fields get committed.


