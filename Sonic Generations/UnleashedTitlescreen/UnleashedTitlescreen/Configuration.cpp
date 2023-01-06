int Configuration::LogoType = 0;
bool Configuration::IgnoreWarnings = false;
std::string Configuration::modPath;
WorldData Configuration::worldData;
std::vector<std::string> Configuration::gensStages;

void Configuration::Read()
{
	INIReader reader(INI_FILE);
	Configuration::LogoType = reader.GetInteger("Appearance", "LogoType", LogoType);
	Configuration::IgnoreWarnings = reader.GetBoolean("General", "IgnoreWarnings", IgnoreWarnings);
	Configuration::gensStages = { "ghz100","ghz200","cpz100","cpz200","ssz100","ssz200","sph100","sph200","cte100", "cte200","ssh100","ssh200","csc100","csc200","euc100","euc200","pla100","pla200"};
	std::ifstream jsonFile(STAGE_LIST_FILE/*, std::ios::in*/);
	
	worldData = WorldData();

	std::vector<std::string> modList;
	Common::GetModIniList(modList);
	modList.insert(modList.end(), ""); //Unleashed title's own stage_list
	
	/// <summary>
	/// What I want to have is a system that parses all the stage_list files from all mods, last being the titlescreen itself, and
	/// if theres more than 1 stage_list but it doesnt override everything, override just the index itself
	/// (thats why there's an Index property)
	/// </summary>
	for (size_t a = 0; a < modList.size(); a++)
	{
		size_t pos = modList.at(a).find_last_of("\\/");
		if (pos != std::string::npos)
		{
			modList.at(a).erase(pos + 1);
		}
		std::ifstream jsonFile(modList.at(a) + STAGE_LIST_FILE);
		if (!jsonFile)
			continue;
		Json::Value root;
		jsonFile >> root;
		auto wd = root["WorldData"];
		Json::Value arrayFlag = wd["FlagData"];
		for (int i = 0; i < arrayFlag.size(); i++)
		{
			worldData.data.push_back(FlagData());
			worldData.data[i].description = std::string(arrayFlag[i]["Description"].asCString());
			worldData.data[i].description = std::string(arrayFlag[i]["Description"].asCString());
			Json::Value element = arrayFlag[i]["LevelData"];
			for (int x = 0; x < element.size(); x++)
			{
				worldData.data[i].data.push_back(LevelData());
				worldData.data[i].data[x].levelID = std::string(element[x]["levelID"].asCString());
				worldData.data[i].data[x].optionName = std::string(element[x]["optionName"].asCString());
				worldData.data[i].data[x].isWhiteWorld = element[x]["isWhiteWorld"].asBool();
				worldData.data[i].data[x].isCapital = element[x]["isCapital"].asBool();
			}
		}		
		break;

	}
	Configuration::worldData = worldData;
}

std::vector<std::string> Configuration::GetAllLevelIDs(bool onlyCustom)
{
	std::vector<std::string> returned;
	for (size_t i = 0; i < Configuration::worldData.data.size(); i++)
	{
		for (size_t x = 0; x < Configuration::worldData.data[i].data.size(); x++)
		{
			if (onlyCustom)
			{
				if (std::find(gensStages.begin(), gensStages.end(), Configuration::worldData.data[i].data[x].levelID) == gensStages.end())
				{
					returned.push_back(Configuration::worldData.data[i].data[x].levelID);
					continue;
				}
			}
			else
				returned.push_back(Configuration::worldData.data[i].data[x].levelID);
		}
	}
	return returned;
}