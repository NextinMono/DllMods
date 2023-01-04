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

	//NEED TO ADD ARCHIVES AFTER <DefAppend>cmn200</DefAppend> AND BEFORE </Node> OF CMN200!!!!!!!!!!!!!!!!!!
	std::string cmn200;
	{
		std::stringstream stream;
		for (auto& node : Configuration::GetAllLevelIDs(true))
		{
			stream << "   <Node>\n";
			stream << "     <Name>" << node << "</Name>\n";
			stream << "     <Archive>" << node << "</Archive>\n";
			stream << "     <Order>" << 0 << "</Order>\n";
			stream << "   </Node>\n";
		}
		cmn200 = stream.str();
	}

	std::string enemydependencies;
	{
		std::stringstream stream;
		for (auto& node : Configuration::GetAllLevelIDs(true))
		{
			stream << "   <DefAppend>\n";
			stream << "     <Name>" << node << "_cmn</Name>\n";
			stream << "     <Archive>EnemyCommon</Archive>\n";
			stream << "     <Archive>EnemyELauncher</Archive>\n";
			stream << "     <Archive>EnemyAeroCannon</Archive>\n";
			stream << "     <Archive>EnemyEFighter</Archive>\n";
			stream << "     <Archive>EnemyBeeton</Archive>\n";
			stream << "     <Archive>EnemySpinner</Archive>\n";
			stream << "     <Archive>EnemyEPawnCommon</Archive>\n";
			stream << "     <Archive>EnemyEPawnSSH</Archive>\n";
			stream << "     <Archive>EnemyMotora</Archive>\n";
			stream << "   </DefAppend>\n";
		}
		enemydependencies = stream.str();
	}
	const size_t newSize = size + str.size() + cmn200.size() /*+ enemydependencies.size()*/;
	const std::unique_ptr<char[]> pBuffer = std::make_unique<char[]>(newSize);

	char* pInsertionPos = strstr(pData, "<Include>");
	char* pInsertionPos_CMN200 = strstr(pData, "<DefAppend>cmn200</DefAppend>") + strlen("<DefAppend>cmn200</DefAppend>")+2;

	// Copy the data from the original buffer into a new buffer
	std::string buffer(pData, size);

	// Insert str into the buffer at pInsertionPos
	buffer.insert(pInsertionPos - pData, str);
	buffer.insert(pInsertionPos_CMN200 - pData, cmn200);

	/*int e = std::string(pData).find("<Name>EnemyCommon</Name>");
	int f = std::string(pData).find("<Name>ghz_cmn</Name>", e);

	buffer.insert(f, enemydependencies);*/
	memcpy(pBuffer.get(), buffer.c_str(), buffer.size());

	bool result;
	{
		result = originalParseArchiveTree(a1, pBuffer.get(), newSize, pDatabase);
	}

	return result;
}

void ArchivePatcher::Install()
{
	archiveDependencies.push_back(ArchiveDependency("egb200", {"pla_cmn", "ghz_cmn", "egb200_cmn"}));
	archiveDependencies.push_back(ArchiveDependency("afr200", { "pla_cmn", "ghz_cmn", "egb200_cmn" }));
	archiveDependencies.push_back(ArchiveDependency("cmn200", { "egb200","afr200" }));
	archiveDependencies.push_back(ArchiveDependency("ExtraUISounds", { "pam000"}));

	INSTALL_HOOK(ParseArchiveTree);
	//INSTALL_HOOK(sub_D4C480);
}