int Configuration::LogoType = 0;

void Configuration::Read()
{
	INIReader reader(INI_FILE);
	Configuration::LogoType = reader.GetBoolean("Appearance", "LogoType", LogoType);
}