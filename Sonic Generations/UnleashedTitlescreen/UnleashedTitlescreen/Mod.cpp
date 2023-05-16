extern "C" __declspec(dllexport) void Init(ModInfo* info)
{
	MessageBoxA(NULL, "", "", 0);

	Configuration::Read();

	if (Common::IsModIdEnabled("bsthlc.generationsd3d11") && !Configuration::IgnoreWarnings)
	{
		MessageBoxA(NULL, "Direct3D 11 might cause a crash with the Unleashed Title Screen enabled, you have been warned!\nIf this doesn't cause a crash, enable the option 'Ignore D3D11 Warning' in the mod manager.", "UnleashedTitleScreen", MB_ICONERROR);
	}

	LetterboxHelper::Initialize(1280, 720);
	std::string modDir = info->CurrentMod->Path;
	size_t pos = modDir.find_last_of("\\/");
	if (pos != std::string::npos)
	{
		modDir.erase(pos + 1);
	}
	Configuration::modPath = modDir;
	//MiniAudioHelper::Initialize();
	////LuanneManager::Install();
	//ArchivePatcher::Install();
	//Title::Install();
	//TitleWorldMap::Install();
	//DecorationTextManager::Install();
	//TitleOption::Install();
	//EnemyTrigger::applyPatches();

	//CSDCommon::Initialize();
}