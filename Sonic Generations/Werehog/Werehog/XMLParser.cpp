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
std::vector<WerehogAttack> attacksToAdd;
WerehogAttack ParseNode(rapidxml::xml_node<>* node, rapidxml::xml_node<>* parent)
{
    WerehogAttack attack = WerehogAttack({ "", { }, { }, {}, {}, 1 });
    for (rapidxml::xml_node<>* child = node->first_node(); child; child = child->next_sibling())
    {
        printf("\n");
        printf(child->name());
         const char* name = child->name();
         auto e = std::string(name);
         trim_cruft(e);
         name = e.c_str();
        //if (isPartOf(name,"ActionName"))
        //    attack.comboName = child->value();
         if (isPartOf(name, "MotionName"))
         {
             attack.animations.push_back(format("evilsonic_attack%s", child->value()));

             attack.comboName = child->value();
        }
        if (isPartOf(name, "KEY__YDown"))
        {
            if (child->value_size() > 0)
            {
                auto val = child->value();
                attack.animations.push_back(format("evilsonic_attack%s", val));
                toCheck = val;
                alreadyChecked = attack.comboName;
                attack.combo.push_back(eKeyState_Y);
            }
        }
        if (isPartOf(name, "KEY__XDown"))
        {
            if (child->value_size() > 0)
            {
                attack.animations.push_back(format("evilsonic_attack%s", child->value()));
                attack.combo.push_back(eKeyState_X);
            }
        }
        if (isPartOf(name, "KEY__ADown"))
        {
            if (child->value_size() > 0)
            {
                attack.animations.push_back(format("evilsonic_attack%s", child->value()));
                attack.combo.push_back(eKeyState_A);
            }
        }
    }
    return attack;
}
void XMLParser::Install()
{
    std::ifstream myfile("EvilAttackAction1.xml");
    rapidxml::xml_document<> doc;

    /* "Read file into vector<char>"  See linked thread above*/
    vector<char> buffer((std::istreambuf_iterator<char>(myfile)), std::istreambuf_iterator<char>());

    buffer.push_back('\0');


    doc.parse<0>(&buffer[0]);
    auto actionNode = doc.first_node()->last_node();
    for (rapidxml::xml_node<>* child = actionNode->first_node(); child; child = child->next_sibling())
    {        
        bool skip = false;

		if (std::string(child->first_node()->value()) == toCheck)
		{
			auto attack = ParseNode(child, actionNode);
			for (size_t i = 0; i < attacksToAdd.size(); i++)
			{
				if (attacksToAdd[i].comboName.find(alreadyChecked) != std::string::npos)
				{
					attacksToAdd[i].combo.insert(attacksToAdd[i].combo.end(), attack.combo.begin(), attack.combo.end());
					attacksToAdd[i].animations.insert(attacksToAdd[i].animations.end(), attack.animations.begin(), attack.animations.end());

					skip = true;
					break;
				}
			}
		}
        else
        {
            if (!skip)
            {
                attacksToAdd.push_back(ParseNode(child, actionNode));
                printf(child->name());
            }
        }
        
    }


}

