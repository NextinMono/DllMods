#pragma once
class HudGate
{
public:
	static void Install();
	static void CreateScreen(Sonic::CGameObject* pParentGameObject);
	static void ToggleScreen(const bool visible, Sonic::CGameObject* pParentGameObject);
	static void KillScreen();
};
