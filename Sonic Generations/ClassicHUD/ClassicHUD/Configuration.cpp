int Configuration::SpriteType = 0;

void Configuration::Read()
{
	INIReader reader(INI_FILE);

	// Main
	Configuration::SpriteType = reader.GetInteger("Main", "SpriteType", SpriteType);
}