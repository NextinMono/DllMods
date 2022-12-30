int Configuration::LogoType = 0;
bool Configuration::IgnoreWarnings = false;
std::string Configuration::modPath;
WorldData Configuration::worldData;
void Configuration::Read()
{
	INIReader reader(INI_FILE);
	Configuration::LogoType = reader.GetInteger("Appearance", "LogoType", LogoType);
	Configuration::IgnoreWarnings = reader.GetBoolean("General", "IgnoreWarnings", IgnoreWarnings);

	std::ifstream jsonFile(STAGE_LIST_FILE/*, std::ios::in*/);
	Json::Value root;
	jsonFile >> root;
	auto wd = root["WorldData"];
	Json::Value& arrayFlag = wd["FlagData"];
	Configuration::worldData = WorldData();
	for (int i = 0; i < arrayFlag.size(); i++)
	{
		worldData.data.push_back(FlagData());
		Json::Value& element = arrayFlag[i]["LevelData"];
		for (int x = 0; x < element.size(); x++)
		{
			worldData.data[i].data.push_back(LevelData());
			worldData.data[i].data[x].levelID = element[x]["levelID"].asString();
			worldData.data[i].data[x].optionName = element[x]["optionName"].asString();
		}
	}
}