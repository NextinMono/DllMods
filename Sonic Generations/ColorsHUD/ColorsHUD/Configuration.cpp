bool Configuration::GaugeShake = true;
std::string Configuration::UIType = "ui";
bool Configuration::XNCPEnabled = true;
float Configuration::GaugeShakeAmountXNCP = 2.5f;

void Configuration::Read()
{
	INIReader reader(INI_FILE);

	// Main
	Configuration::UIType = reader.Get("Main", "IncludeDir2", UIType);
	Configuration::GaugeShake = reader.GetBoolean("Main", "GaugeShake", GaugeShake);
	Configuration::XNCPEnabled = reader.GetBoolean("XNCP", "XNCPEnabled", XNCPEnabled);
	Configuration::GaugeShakeAmountXNCP = reader.GetFloat("XNCP", "GaugeShakePower", GaugeShakeAmountXNCP);
}