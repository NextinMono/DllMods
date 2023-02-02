#pragma once

#define INI_FILE "mod.ini"
#define STAGE_LIST_FILE "stage_list.json"
#define MOD_PATH ""

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
	std::vector<LevelData> dataNight;
};
struct WorldData
{
	std::vector<FlagData> data;
};
enum TitleType {
	Retail,
	Preview,
	E3
};
class Configuration
{
	
public:
	/// <summary>
	/// Reads the INI configuration file for the mod.
	/// </summary>
	static void Read();
	static std::vector<std::string> GetAllLevelIDs(bool onlyCustom);
	static std::vector<std::string> GetAllWhiteWorld();
	static int GetCapital(int flagID);
	static int LogoType;
	static bool IgnoreWarnings;
	static TitleType MenuType;
	static bool CompatibilityMode;
	static std::string modPath;
	static WorldData worldData; 
	static std::vector<std::string> gensStages;



};

