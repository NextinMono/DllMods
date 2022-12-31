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
	const size_t newSize = size + str.size() + cmn200.size();
	const std::unique_ptr<char[]> pBuffer = std::make_unique<char[]>(newSize);
	//memcpy(pBuffer.get(), pData, size);


	char* pInsertionPos = strstr(pData, "<Include>");
	char* pInsertionPos_CMN200 = strstr(pData, "<DefAppend>cmn200</DefAppend>") + strlen("<DefAppend>cmn200</DefAppend>")+2;

	// Copy the data from the original buffer into a new buffer
	std::string buffer(pData, size);

	// Insert str into the buffer at pInsertionPos
	buffer.insert(pInsertionPos - pData, str);
	buffer.insert(pInsertionPos_CMN200 - pData, cmn200);

	memcpy(pBuffer.get(), buffer.c_str(), buffer.size());

	bool result;
	{
		result = originalParseArchiveTree(a1, pBuffer.get(), newSize, pDatabase);
	}

	return result;
}

void ArchivePatcher::Install()
{
	archiveDependencies.push_back(ArchiveDependency("egb200", { }));
	archiveDependencies.push_back(ArchiveDependency("afr200", {  }));
	archiveDependencies.push_back(ArchiveDependency("cmn200", { "egb200","afr200" }));

	INSTALL_HOOK(ParseArchiveTree);
	//INSTALL_HOOK(sub_D4C480);
}