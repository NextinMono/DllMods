int Configuration::SpriteType = 0;
bool Configuration::LoadingEnabled = false;

void Configuration::Read()
{
	INIReader reader(INI_FILE);

	// Main
	Configuration::SpriteType = reader.GetInteger("Main", "SpriteType", SpriteType);
	Configuration::LoadingEnabled = reader.Get("Main", "IncludeDir1", "") != "";
}