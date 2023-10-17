bool Configuration::InfiniteBoost = false;
bool Configuration::GearsKeysEnabled = true;
bool  Configuration::SkillsEnabled = true;
int Configuration::GearsCount = 0;
int Configuration::KeysCount = 0;
int Configuration::RingCapCount = 999;
std::string Configuration::HUDType = "full";
bool Configuration::IsCyberspaceHUD = false;
bool Configuration::LivesEnabled = true;
bool Configuration::SpeedometerEnabled = true;
bool Configuration::XPNumberEnabled = true;
std::string Configuration::ScoreGenerationsFormat = "%d";

void Configuration::Read()
{
	INIReader reader(INI_FILE);

	// Main
	Configuration::InfiniteBoost = reader.GetBoolean("Main", "InfiniteBoost", InfiniteBoost);
	Configuration::HUDType = reader.Get("Main", "Preset", HUDType);

	//Additional
	Configuration::GearsKeysEnabled = reader.GetBoolean("Additional", "GearsKeysEnabled", GearsKeysEnabled);
	Configuration::LivesEnabled = reader.GetBoolean("Additional", "LivesEnabled", LivesEnabled);
	Configuration::SkillsEnabled = reader.GetBoolean("Additional", "SkillsEnabled", SkillsEnabled);
	Configuration::SpeedometerEnabled = reader.GetBoolean("Additional", "SpeedometerEnabled", SpeedometerEnabled);
	Configuration::XPNumberEnabled = reader.GetBoolean("Additional", "SpeedometerNumberEnabled", XPNumberEnabled);

	//Count
	Configuration::GearsCount = reader.GetInteger("Count", "GearsCount", GearsCount);
	Configuration::KeysCount = reader.GetInteger("Count", "KeysCount", KeysCount);
	Configuration::RingCapCount = reader.GetInteger("Count", "MaxRings", RingCapCount);
	Configuration::ScoreGenerationsFormat = reader.Get("Count", "ScoreGenerationsFormat", ScoreGenerationsFormat);

	if (HUDType == "openzone") 
	{
		Configuration::GearsKeysEnabled = true;
		Configuration::LivesEnabled = true;
		Configuration::SkillsEnabled = true;
		Configuration::SpeedometerEnabled = true;
		Configuration::XPNumberEnabled = true;
		Configuration::GearsCount = 0;
		Configuration::KeysCount = 0;
		Configuration::RingCapCount = 999;
	}
	if (HUDType == "cyberspace")
	{
		Configuration::GearsKeysEnabled = false;
		Configuration::LivesEnabled = false;
		Configuration::SkillsEnabled = false;
		Configuration::SpeedometerEnabled = false;
		Configuration::XPNumberEnabled = false;
		Configuration::GearsCount = 0;
		Configuration::KeysCount = 0;
		Configuration::RingCapCount = 999;

	}
}