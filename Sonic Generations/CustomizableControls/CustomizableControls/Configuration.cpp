int Configuration::BoostButton = 0;

void Configuration::Read()
{
	INIReader reader(INI_FILE);
	Configuration::BoostButton = reader.GetInteger("Controls", "BoostButton", BoostButton);
}