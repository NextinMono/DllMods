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
	auto list = Configuration::GetAllLevelIDs(false);
	list.push_back("pam000");
	//NEED TO ADD ARCHIVES AFTER <DefAppend>cmn200</DefAppend> AND BEFORE </Node> OF CMN200!!!!!!!!!!!!!!!!!!
	std::string cmn200;
	{
		std::stringstream stream;
		for (auto& node : list)
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
		for (auto& node : list)
		{
			stream << "   <DefAppend>\n";
			stream << "     <Name>" << node << "</Name>\n";
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
	const size_t newSize = size + str.size() + cmn200.size() + enemydependencies.size();
	const std::unique_ptr<char[]> pBuffer = std::make_unique<char[]>(newSize);


	// Copy the data from the original buffer into a new buffer
	std::string buffer(pData, size);


	//As a compromise, ive added a bunch of comments in the archivetree xml, we need to find a better way to do this without needing these comments.

	//v https://www.youtube.com/watch?v=bgs9OhjAE2g v
	auto pInsertionPos_enemy = buffer.find("<!--SWATSWM ENTRY-->");
	buffer.insert(pInsertionPos_enemy, enemydependencies);

	auto pInsertionPos = buffer.find("<!--INCLUDE ENTRY-->");
	buffer.insert(pInsertionPos, str);
	auto pInsertionPos_CMN200 = buffer.find("<!--SWATSWM-CMN2 ENTRY-->");
	buffer.insert(pInsertionPos_CMN200, cmn200);
	memcpy(pBuffer.get(), buffer.c_str(), buffer.size());

	bool result;
	{
		result = originalParseArchiveTree(a1, pBuffer.get(), newSize, pDatabase);
	}

	return result;
}

void ArchivePatcher::Install()
{

	

	//rawData = database->GetRawData("test.txt");
	archiveDependencies.push_back(ArchiveDependency("egb200", {}));
	archiveDependencies.push_back(ArchiveDependency("afr200", {}));
	archiveDependencies.push_back(ArchiveDependency("cmn200", { }));

	INSTALL_HOOK(ParseArchiveTree);
	//INSTALL_HOOK(sub_D4C480);
}

void ArchivePatcher::LoadExtraUISounds()
{
}