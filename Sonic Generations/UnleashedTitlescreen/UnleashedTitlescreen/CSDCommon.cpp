std::vector<Chao::CSD::CScene*> CSDCommon::scenesPlayingBack; //idk why i need to do this, symbol errors-
HOOK(void*, __fastcall, CheckScenesBack, 0xE7BED0, void* This, void* Edx, float elapsedTime, uint8_t a3)
{
	if (CSDCommon::scenesPlayingBack.size() != 0)
	{
		for (size_t i = 0; i < CSDCommon::scenesPlayingBack.size(); i++)
		{
			if (i > CSDCommon::scenesPlayingBack.size())
				break;
			if (!CSDCommon::scenesPlayingBack[i])
			{
				std::vector<Chao::CSD::CScene*>::iterator scene = CSDCommon::scenesPlayingBack.begin() + i;
				CSDCommon::scenesPlayingBack.erase(scene);
				continue;
			}
			else
			{
				Common::ClampFloat(CSDCommon::scenesPlayingBack[i]->m_MotionFrame, 0, CSDCommon::scenesPlayingBack[i]->m_MotionEndFrame);

				if (CSDCommon::scenesPlayingBack[i]->m_MotionFrame <= 0 || CSDCommon::scenesPlayingBack[i]->m_MotionFrame >= CSDCommon::scenesPlayingBack[i]->m_MotionEndFrame)
				{
					CSDCommon::FreezeMotion(CSDCommon::scenesPlayingBack[i], 0);
					std::vector<Chao::CSD::CScene*>::iterator scene = CSDCommon::scenesPlayingBack.begin() + i;
					CSDCommon::scenesPlayingBack.erase(scene);
					continue;
				}
			}
		}
	}
	return originalCheckScenesBack(This, Edx, elapsedTime, a3);
}

void CSDCommon::SplitTextToSeparateCasts(Chao::CSD::CScene* scene, const char* formatCastName, const char* text, int maxCharacterPerLine, int maxLines)
{
	const string input = string(text);
	std::stringstream ss(input);
	string line;
	vector<string> lines;
	for (std::string line; std::getline(ss, line);)
	{
		lines.push_back(line);
	}

	if (lines.empty())
		lines.emplace_back();

	char buff[32];
	for (size_t i = 0; i < maxLines; i++)
	{
		sprintf(buff, formatCastName, i);
		if (i <= lines.size() - 1)
		{
			scene->GetNode(buff)->SetText(lines.at(i).c_str());
		}
	}
}
void CSDCommon::Initialize()
{
	INSTALL_HOOK(CheckScenesBack);
}