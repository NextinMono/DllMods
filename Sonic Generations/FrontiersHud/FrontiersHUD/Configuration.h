#pragma once

#define INI_FILE "mod.ini"

class Configuration
{
public:
	/// <summary>
	/// Reads the INI configuration file for the mod.
	/// </summary>
	static void Read();

	//Disables everything that isnt related to the boost bar. Set this to true or false before compiling tho.
	static bool IsCyberspaceHUD;

	static bool InfiniteBoost;
	static bool GearsKeysEnabled;
	static bool SpeedometerEnabled;
	static bool SpeedometerNumberEnabled;
	static bool LivesEnabled;
	static bool SkillsEnabled;
	static int GearsCount;
	static int KeysCount;
	static int RingCapCount;
	static std::string HUDType;
	static std::string ScoreGenerationsFormat;


	
	static bool IsBeta;
};

