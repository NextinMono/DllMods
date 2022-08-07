std::string Configuration::UIType = "scu";
bool Configuration::IsColorsOG = false;
Configuration::ButtonStyle Configuration::ButtonType = ButtonStyle::Xbox;


void Configuration::Read()
{
	INIReader reader(INI_FILE);

	Configuration::UIType = reader.Get("Main", "IncludeDir2", UIType);
	Configuration::IsColorsOG = /*Configuration::UIType == "2010"*/ true;
	/*ButtonType = (ButtonStyle)reader.GetInteger("Appearance", "buttonType", (int)ButtonType);*/
}