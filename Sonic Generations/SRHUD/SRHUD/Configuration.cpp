bool Configuration::IsBeta = false;

void Configuration::Read()
{
	INIReader reader(INI_FILE);

	// Main
	Configuration::IsBeta = reader.GetBoolean("Main", "Wildfire", IsBeta);
}