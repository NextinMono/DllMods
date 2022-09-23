extern "C" __declspec(dllexport) void Init(const char* path)
{
	LetterboxHelper::Initialize(1280, 720);
	Configuration::Read();
	HudSonicStage::Install();
}
extern "C" __declspec(dllexport) void PostInit()
{
	ScoreGenerationsAPI::SetVisibility(false);
}

__declspec(dllexport) ModInfo GensModInfo = { ModLoaderVer, GameVer };
