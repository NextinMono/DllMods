int Configuration::LogoType = 0;
bool Configuration::IgnoreWarnings = false;
const char* Configuration::modPath;
void Configuration::Read()
{
	INIReader reader(INI_FILE);
	Configuration::LogoType = reader.GetInteger("Appearance", "LogoType", LogoType);
	Configuration::IgnoreWarnings = reader.GetBoolean("General", "IgnoreWarnings", IgnoreWarnings);
}