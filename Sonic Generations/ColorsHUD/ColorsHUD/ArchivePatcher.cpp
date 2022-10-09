//Taken directly from UnleashedHUD, with some changes
std::vector<ArchiveDependency> ArchivePatcher::archiveDependencies = {};

HOOK(bool, __stdcall, ParseArchiveTree, 0xD4C8E0, void* a1, char* pData, const size_t size, void* pDatabase)
{
	std::string str;
	{
		std::stringstream stream;

		for (auto& node : ArchivePatcher::archiveDependencies)
		{
			stream << "  <Node>\n";
			stream << "    <Name>" << node.archive << "</Name>\n";
			stream << "    <Archive>" << node.archive << "</Archive>\n";
			stream << "    <Order>" << 0 << "</Order>\n";
			stream << "    <DefAppend>" << node.archive << "</DefAppend>\n";

			for (auto& archive : node.dependencies)
			{
				stream << "    <Node>\n";
				stream << "      <Name>" << archive << "</Name>\n";
				stream << "      <Archive>" << archive << "</Archive>\n";
				stream << "      <Order>" << 0 << "</Order>\n";
				stream << "    </Node>\n";
			}

			stream << "  </Node>\n";
		}

		str = stream.str();
	}

	const size_t newSize = size + str.size();
	const std::unique_ptr<char[]> pBuffer = std::make_unique<char[]>(newSize);
	memcpy(pBuffer.get(), pData, size);

	char* pInsertionPos = strstr(pBuffer.get(), "<Include>");

	memmove(pInsertionPos + str.size(), pInsertionPos, size - (size_t)(pInsertionPos - pBuffer.get()));
	memcpy(pInsertionPos, str.c_str(), str.size());

	bool result;
	{
		result = originalParseArchiveTree(a1, pBuffer.get(), newSize, pDatabase);
	}

	return result;
}

void ArchivePatcher::Install()
{
	//std::vector<std::string> dependencies = { "SonicActionCommon", "SonicActionCommonHud", "SystemCommon", "cmn200", "Guide", "pam_cmn", "StageGate", "Title", "NoticeBoard", "pam000" };
	switch (Configuration::ButtonType)
	{
	case Configuration::ButtonStyle::Xbox:
		archiveDependencies.push_back(ArchiveDependency("XboxSeriesButtons", { "SystemCommon", "HowTo"}));
		WRITE_STRING(0x1688344, "ui_howxb");
		WRITE_STRING(0x16886A8, "ui_howxb");
		WRITE_STRING(0x1692BC4, "ui_howxb");
		break;

	case Configuration::ButtonStyle::Playstation:
		archiveDependencies.push_back(ArchiveDependency("PlaystationButtons", { "SystemCommon", "HowTo" }));
		WRITE_STRING(0x1688344, "ui_howps");
		WRITE_STRING(0x16886A8, "ui_howps");
		WRITE_STRING(0x1692BC4, "ui_howps");
		break;

	case Configuration::ButtonStyle::Switch:
		archiveDependencies.push_back(ArchiveDependency("SwitchButtons", { "SystemCommon", "HowTo" }));
		WRITE_STRING(0x1688344, "ui_howns");
		WRITE_STRING(0x16886A8, "ui_howns");
		WRITE_STRING(0x1692BC4, "ui_howns");
		break;

	case Configuration::ButtonStyle::Wii:
		archiveDependencies.push_back(ArchiveDependency("WiiButtons", { "SystemCommon", "HowTo" }));
		WRITE_STRING(0x1688344, "ui_howrv");
		WRITE_STRING(0x16886A8, "ui_howrv");
		WRITE_STRING(0x1692BC4, "ui_howrv");
		break;

	default:
		break;
	}

	if (Configuration::IsColorsOG)
	{
		archiveDependencies.push_back(ArchiveDependency("WiiTextures", { "SonicActionCommonHud" }));
	}
	//const char* loadingWordArchive = "LoadingGens";
	//if (Common::IsModNameEnabled("Unleashed Project"))
	//{
	//	loadingWordArchive = "LoadingUP";
	//}
	//if (Common::IsModNameEnabled("Colors Project"))
	//{
	//	loadingWordArchive = "LoadingColors";
	//}
	//if (Common::DoesArchiveExist("LoadingCustom.ar.00"))
	//{
	//	loadingWordArchive = "LoadingCustom";
	//}


	//archiveDependencies.push_back(ArchiveDependency(loadingWordArchive, { "Loading" }));
	archiveDependencies.push_back(ArchiveDependency("ResultColors", { "SonicActionCommonHud" }));
	INSTALL_HOOK(ParseArchiveTree);
}