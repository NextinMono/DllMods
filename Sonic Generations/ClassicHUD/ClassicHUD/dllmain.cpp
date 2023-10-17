extern "C" __declspec(dllexport) void Init(const char* path)
{
	MessageBoxExA(0, "", "", MB_OK, 0);

	Configuration::Read();
	ArchivePatcher::Install();
	HudSonicStage::Install();
	if(Configuration::LoadingEnabled)
	HudLoading::Install();
}