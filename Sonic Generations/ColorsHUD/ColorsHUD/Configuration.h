#pragma once

#define INI_FILE "mod.ini"

class Configuration
{
public:
	/// <summary>
	/// Reads the INI configuration file for the mod.
	/// </summary>
	static void Read();

	static bool IsColorsOG;
	/// <summary>
	/// The current UI type being used.
	/// </summary>
	static std::string UIType;
	static enum class ButtonStyle : int {
		Xbox,
		Playstation,
		Switch,
		Wii
	};

	static std::string stateNameDebug;
	static ButtonStyle ButtonType;
	
};

