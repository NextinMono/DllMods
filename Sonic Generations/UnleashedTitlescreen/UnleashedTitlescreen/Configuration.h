#pragma once

#define INI_FILE "mod.ini"
#define STAGE_LIST_FILE "stage_list.json"

struct LevelData
{
	std::string levelID;
	std::string optionName;
	bool isWhiteWorld;
	bool isCapital;
};
struct FlagData
{
	std::string description;
	std::vector<LevelData> data;
};
struct WorldData
{
	std::vector<FlagData> data;
};
class Configuration
{
	
public:
	/// <summary>
	/// Reads the INI configuration file for the mod.
	/// </summary>
	static void Read();
	static std::vector<std::string> GetAllLevelIDs(bool onlyCustom);
	static int LogoType;
	static bool IgnoreWarnings;
	static std::string modPath;
	static WorldData worldData;


};

