std::string Configuration::UIType = "scu";
bool Configuration::IsColorsOG;
Configuration::ButtonStyle Configuration::ButtonType = ButtonStyle::Xbox;

void Configuration::Read()
{
	INIReader reader(INI_FILE);

	Configuration::UIType = reader.Get("Main", "IncludeDir4", UIType);
	Configuration::IsColorsOG = reader.GetBoolean("Customization", "HDColors", IsColorsOG);
	Configuration::ButtonType = (ButtonStyle)reader.GetInteger("Appearance", "buttonType", (int)ButtonType);
}