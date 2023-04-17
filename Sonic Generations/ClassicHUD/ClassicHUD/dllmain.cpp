extern "C" __declspec(dllexport) void Init(const char* path)
{
	Configuration::Read();
	ArchivePatcher::Install();
	HudSonicStage::Install();
	HudLoading::Install();
}