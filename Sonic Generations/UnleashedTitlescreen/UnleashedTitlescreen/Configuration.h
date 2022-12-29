#pragma once

#define INI_FILE "mod.ini"
#define STAGE_LIST_FILE "stage_list.json"

class Configuration
{
public:
	/// <summary>
	/// Reads the INI configuration file for the mod.
	/// </summary>
	static void Read();

	static int LogoType;
	static bool IgnoreWarnings;
	static const char* modPath;
};

