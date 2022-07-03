extern "C" __declspec(dllexport) void Init(const char* path)
{
	Configuration::Read();
	MessageBoxExA(0, "Before you comment or give issues about the mod, I'll list what I already know about: \n1. The boost bar shake isn't the same as colors, might try to make it as close as possible if I can.\n3. Skills in the wisp container might look weird or with slight transparent pixels, this is because its a pain.\nHave fun!", "Sonic Colors Ultimate HUD Port - NextinHKRY", MB_OK, 0);

	if (Configuration::XNCPEnabled == true)
	{
#if DEBUG
		Title::Install();
#endif
		HudPause::Install();
		HudBtnGuide::Install();
	}
	//Install UI anyway, as it has patches. It'll disable the XNCP in this code anyway if its disabled
	HudSonicStage::Install();

	HudResult::Install();
}
extern "C" __declspec(dllexport) void PostInit()
{
	ScoreGenerationsAPI::SetVisibility(false);
}

__declspec(dllexport) ModInfo GensModInfo = { ModLoaderVer, GameVer };
