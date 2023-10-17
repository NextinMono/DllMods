
extern "C" __declspec(dllexport) void Init(const char* path)
{
	MessageBoxExA(0, "","", MB_OK, 0);

	LetterboxHelper::Initialize(1280, 720);
	Configuration::Read();
	ArchivePatcher::Install();
	HudSonicStage::Install();
	HudResult::Install();
}
extern "C" __declspec(dllexport) void PostInit()
{
	ScoreGenerationsAPI::SetVisibility(false);
}
extern "C" void __declspec(dllexport) OnFrame()
{
	CSDCommon::update();
}

