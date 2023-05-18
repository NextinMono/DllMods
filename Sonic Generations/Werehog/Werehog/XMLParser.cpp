#include <iostream>
#include <fstream>
#include <vector>
using namespace Sonic;
void trim_cruft(std::string& buffer)
{
	static const char cruft[] = "\n\r";

	buffer.erase(buffer.find_last_not_of(cruft) + 1);
}
bool isPartOf(const char* w1, const char* w2)
{
	int i = 0;
	int j = 0;


	while (w1[i] != '\0') {
		if (w1[i] == w2[j])
		{
			int init = i;
			while (w1[i] == w2[j] && w2[j] != '\0')
			{
				j++;
				i++;
			}
			if (w2[j] == '\0') {
				return true;
			}
			j = 0;
		}
		i++;
	}
	return false;
}

std::string toCheck;
std::string alreadyChecked;
std::vector<Motion> XMLParser::animationTable;
std::vector<std::string> alreadyRegistered;
std::vector<WerehogAttackNew> XMLParser::attacks;
std::vector<WerehogAttackNew> XMLParser::starterAttacks;
WerehogAttackNew ParseActionNode(rapidxml::xml_node<>* node, rapidxml::xml_node<>* parent)
{
	WerehogAttackNew attack = WerehogAttackNew();
	for (rapidxml::xml_node<>* child = node->first_node(); child; child = child->next_sibling())
	{
		const char* name = child->name();
		std::stringstream ss(child->value());
		if (isPartOf(name, "ActionName"))
			attack.ActionName = std::string(child->value());
		if (isPartOf(name, "MotionName"))
			attack.MotionName = std::string(child->value());
		if (isPartOf(name, "ValidLevel_Min"))
			attack.ValidLevel_Min = reinterpret_cast<int>(child->value());
		if (isPartOf(name, "ValidLevel_Max"))
			attack.ValidLevel_Max = reinterpret_cast<int>(child->value());
		if (isPartOf(name, "ValidCommon"))
			attack.ValidCommon = child->value() == "true" ? true : false;
		if (isPartOf(name, "ValidBerserker"))
			attack.ValidBerserker = child->value() == "true" ? true : false;
		if (isPartOf(name, "KEY__YDown"))
			attack.KEY__YDown = std::string(child->value());
		if (isPartOf(name, "KEY__XDown"))
			attack.KEY__YDown = std::string(child->value());
		if (isPartOf(name, "KEY__ADown"))
			attack.KEY__YDown = std::string(child->value());
		if (isPartOf(name, "KEY__Land"))
			attack.KEY__YDown = std::string(child->value());
		if (isPartOf(name, "KEY__AirCombo"))
			attack.KEY__YDown = std::string(child->value());
		if (isPartOf(name, "KEY__End"))
			attack.KEY__YDown = std::string(child->value());
		if (isPartOf(name, "Guard"))
			attack.Guard = child->value() == "true" ? true : false;
		if (isPartOf(name, "Avoid"))
			attack.Avoid = child->value() == "true" ? true : false;
		if (isPartOf(name, "KEY__StartFrame"))
			attack.KEY__StartFrame = reinterpret_cast<int>(child->value());
		if (isPartOf(name, "KEY__EndFrame"))
			attack.KEY__EndFrame = reinterpret_cast<int>(child->value());
		if (isPartOf(name, "WaitEndMotionEndFrame"))
			attack.WaitEndMotionEndFrame = reinterpret_cast<int>(child->value());
		if (isPartOf(name, "WaitEndMotionSpeed"))
			attack.WaitEndMotionSpeed = reinterpret_cast<int>(child->value());
		if (isPartOf(name, "EndMotionSpeed"))
			attack.EndMotionSpeed = reinterpret_cast<int>(child->value());
		if (isPartOf(name, "LandStartFrame"))
			attack.LandStartFrame = reinterpret_cast<int>(child->value());
		if (isPartOf(name, "ActionValidHeightMin"))
			attack.ActionValidHeightMin = reinterpret_cast<int>(child->value());


		//printf("\n");
		//printf(child->name());
		// const char* name = child->name();
		// auto e = std::string(name);
		// trim_cruft(e);
		// name = e.c_str();
		////if (isPartOf(name,"ActionName"))
		////    attack.comboName = child->value();
		// if (isPartOf(name, "MotionName"))
		// {
		//     attack.animations.push_back(format("evilsonic_attack%s", child->value()));

		//     attack.comboName = child->value();
		//}
		//if (isPartOf(name, "KEY__YDown"))
		//{
		//    if (child->value_size() > 0)
		//    {
		//        auto val = child->value();
		//        attack.animations.push_back(format("evilsonic_attack%s", val));
		//        toCheck = val;
		//        alreadyChecked = attack.comboName;
		//        attack.combo.push_back(eKeyState_Y);
		//    }
		//}
		//if (isPartOf(name, "KEY__XDown"))
		//{
		//    if (child->value_size() > 0)
		//    {
		//        attack.animations.push_back(format("evilsonic_attack%s", child->value()));
		//        attack.combo.push_back(eKeyState_X);
		//    }
		//}
		//if (isPartOf(name, "KEY__ADown"))
		//{
		//    if (child->value_size() > 0)
		//    {
		//        attack.animations.push_back(format("evilsonic_attack%s", child->value()));
		//        attack.combo.push_back(eKeyState_A);
		//    }
		//}
	}
	return attack;
}

Motion ParseMotionNode(rapidxml::xml_node<>* node)
{
	Motion returned;
	for (rapidxml::xml_node<>* child = node->first_node(); child; child = child->next_sibling())
	{
		const char* name = child->name();
		std::stringstream ss(child->value());
		if (isPartOf(name, "MotionName"))
			returned.MotionName = child->value();
		if (isPartOf(name, "FileName"))
			returned.FileName = child->value();
		if (isPartOf(name, "MotionFirstSpeed")) //dont think about it too much
			returned.MotionFirstSpeed = reinterpret_cast<int>(child->value());
	}
	return returned;
}
void RegisterAnims(std::vector<Motion> vec)
{
	for (size_t i = 0; i < vec.size(); i++)
	{
		auto attack = vec[i];
		printf((std::string("\n") + std::format("evilsonic_attack{0}", attack.FileName)).c_str());
		std::string file = std::format("evilsonic_attack{0}", attack.FileName);
		std::string fileU = std::format("Evilsonic_attack{0}", attack.FileName);
		CustomAnimationManager::RegisterAnimation(fileU, file);
		vec[i].FileName = file;
		if (std::find(alreadyRegistered.begin(), alreadyRegistered.end(), attack.MotionName) == alreadyRegistered.end())
		{
			alreadyRegistered.push_back(vec[i].MotionName);
		}
		
	}
}
void XMLParser::Install()
{
	std::ifstream myfile("EvilAttackAction1.xml");
	rapidxml::xml_document<> doc;

	/* "Read file into vector<char>"  See linked thread above*/
	vector<char> buffer((std::istreambuf_iterator<char>(myfile)), std::istreambuf_iterator<char>());

	buffer.push_back('\0');


	doc.parse<0>(&buffer[0]);
	auto motionNode = doc.first_node()->first_node()->next_sibling()->next_sibling();
	for (rapidxml::xml_node<>* child = motionNode->first_node(); child; child = child->next_sibling())
	{
		XMLParser::animationTable.push_back(ParseMotionNode(child));
	}
	auto actionNode = doc.first_node()->last_node();
	for (rapidxml::xml_node<>* child = actionNode->first_node(); child; child = child->next_sibling())
	{
		bool skip = false;

		auto attack = ParseActionNode(child, actionNode);
		if (attack.ActionName.find("Start_") != std::string::npos)
			XMLParser::starterAttacks.push_back(attack);
		else
			XMLParser::attacks.push_back(attack);

	}
	RegisterAnims(XMLParser::animationTable);
	printf("\nParsing Complete");


}


