extern "C" __declspec(dllexport) void Init(const char* path)
{
	Configuration::Read();
	ArchivePatcher::Install();
	SonicPlugin::Install();
}
__declspec(dllexport) ModInfo GensModInfo = { ModLoaderVer, GameVer };
