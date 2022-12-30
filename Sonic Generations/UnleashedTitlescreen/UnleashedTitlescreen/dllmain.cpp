extern "C" __declspec(dllexport) void Init(ModInfo* modInfo)
{
	MessageBoxA(NULL, "", "", 0);

	Configuration::Read();

	if (Common::IsModIdEnabled("bsthlc.generationsd3d11") && !Configuration::IgnoreWarnings)
	{
		MessageBoxA(NULL, "Direct3D 11 might cause a crash with the Unleashed Title Screen enabled, you have been warned!\nIf this doesn't cause a crash, enable the option 'Ignore D3D11 Warning' in the mod manager.", "UnleashedTitleScreen", MB_ICONERROR);
	}

	std::string modDir = modInfo->CurrentMod->Path;
	size_t pos = modDir.find_last_of("\\/");
	if (pos != std::string::npos)
	{
		modDir.erase(pos + 1);
	}
	Configuration::modPath = modDir + STAGE_LIST_FILE;
	ArchivePatcher::Install();
	LetterboxHelper::Initialize(1280, 720);
	Title::Install();
	TitleWorldMap::Install();
	TitleOption::Install();
	CSDCommon::Initialize();
}
//__declspec(dllexport) ModInfo GensModInfo = { { ModLoaderVer, GameVer }; }
