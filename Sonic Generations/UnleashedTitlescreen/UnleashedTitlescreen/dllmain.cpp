extern "C" __declspec(dllexport) void Init(const char* path)
{
	if (Common::IsModIdEnabled("bsthlc.generationsd3d11"))
	{
		MessageBoxA(NULL, "Direct3D 11 WILL cause a crash with Unleashed Title Screen enabled, you have been warned!\nIf this doesn't cause a crash, tell Nextin about it.", "UnleashedTitleScreen", MB_ICONERROR);
	}
	Configuration::Read();
	ArchivePatcher::Install();
	Title::Install();
	TitleOption::Install();
	CSDCommon::Initialize();
}
__declspec(dllexport) ModInfo GensModInfo = { ModLoaderVer, GameVer };
