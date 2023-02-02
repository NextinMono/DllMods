#pragma once
struct DecoTextInfo
{
	const char* text;
	std::vector<Chao::CSD::RCPtr<Chao::CSD::CScene>> lines;
	DecoTextInfo(const char* txt)
	{
		text = txt;
	}
};
class DecorationTextManager
{

public:

	static void Install();
	static void CreateScreen(Sonic::CGameObject* pParentGameObject);
	static void ToggleScreen(const bool visible, Sonic::CGameObject* pParentGameObject);
	static void KillScreen();
	static DecoTextInfo InitializeText(const char* text, float spacing, int alignment, Hedgehog::math::CVector position);

};
