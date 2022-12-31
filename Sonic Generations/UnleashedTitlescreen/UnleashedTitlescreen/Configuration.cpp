int Configuration::LogoType = 0;
bool Configuration::IgnoreWarnings = false;
std::string Configuration::modPath;
WorldData Configuration::worldData;

std::vector<std::string> gensStages = { "ghz200", "cpz200", "ssz200","sph200", "cte200", "ssh200", "csc200", "euc200", "pla200", "ghz100", "cpz100", "ssz100","sph100", "cte100", "ssh100", "csc100", "euc100", "pla100" };
void Configuration::Read()
{
	INIReader reader(INI_FILE);
	Configuration::LogoType = reader.GetInteger("Appearance", "LogoType", LogoType);
	Configuration::IgnoreWarnings = reader.GetBoolean("General", "IgnoreWarnings", IgnoreWarnings);

	std::ifstream jsonFile(STAGE_LIST_FILE/*, std::ios::in*/);
	Json::Value root;
	jsonFile >> root;
	auto wd = root["WorldData"];
	Json::Value arrayFlag = wd["FlagData"];
	worldData = WorldData();
	for (int i = 0; i < arrayFlag.size(); i++)
	{
		worldData.data.push_back(FlagData());
		Json::Value element = arrayFlag[i]["LevelData"];
		for (int x = 0; x < element.size(); x++)
		{
			worldData.data[i].data.push_back(LevelData());
			worldData.data[i].data[x].levelID = std::string(element[x]["levelID"].asCString());
			worldData.data[i].data[x].optionName = std::string(element[x]["optionName"].asCString());
		}
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