
extern "C" __declspec(dllexport) void Init(const char* path)
{
	MessageBoxExA(0, "Before you comment or give issues about the mod, I'll list what I already know about: \n1. The boost bar shake isn't the same as colors, might try to make it as close as possible if I can.\n3. Skills in the wisp container might look weird or with slight transparent pixels, this is because its a pain.\nHave fun!", "Sonic Colors Ultimate HUD Port - NextinHKRY", MB_OK, 0);

	CSDCommon::Initialize();
	Configuration::Read();
	ArchivePatcher::Install();
	

	HudPause::Install();
	HudBtnGuide::Install();
	HudSonicStage::Install();
	HudLoading::Install();
	HudResult::Install();
	//Title::Install();
	HudGate::Install();
	/*TestingCode::Install();*/
}
extern "C" __declspec(dllexport) void PostInit()
{
	ScoreGenerationsAPI::SetVisibility(false);
}

__declspec(dllexport) ModInfo GensModInfo = { ModLoaderVer, GameVer };
