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
	//archiveDependencies.push_back(ArchiveDependency(loadingWordArchive, { "Loading" }));
	switch (Configuration::SpriteType)
	{
	case 1:
	{
		archiveDependencies.push_back(ArchiveDependency("Sonic2", { "SonicActionCommonHud" }));
		archiveDependencies.push_back(ArchiveDependency("LoadingS2", { "Loading" }));
		
		
		break;
	}
	case 2:
	{
		archiveDependencies.push_back(ArchiveDependency("Sonic3", { "SonicActionCommonHud" }));
		break;
	}
	case 3:
	{
		archiveDependencies.push_back(ArchiveDependency("SonicCD", { "SonicActionCommonHud" }));
		break;
	}

	}
	INSTALL_HOOK(ParseArchiveTree);
}