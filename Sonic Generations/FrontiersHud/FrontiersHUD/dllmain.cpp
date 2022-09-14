extern "C" __declspec(dllexport) void Init(const char* path)
{
	Configuration::Read();
	HudSonicStage::Install();
}
extern "C" __declspec(dllexport) void PostInit()
{
	ScoreGenerationsAPI::SetVisibility(false);
}

__declspec(dllexport) ModInfo GensModInfo = { ModLoaderVer, GameVer };
