
Chao::CSD::RCPtr<Chao::CSD::CProject> rcDeco;
boost::shared_ptr<Sonic::CGameObjectCSD> spDeco;
std::vector<DecoTextInfo> DecoInfoCache;


void __fastcall DecoTextRemoveCallback(Sonic::CGameObject* This, void*, Sonic::CGameDocument* pGameDocument)
{
	DecorationTextManager::KillScreen();


	for (size_t i = 0; i < DecoInfoCache.size(); i++)
	{
		for (size_t i = 0; i < DecoInfoCache.at(DecoInfoCache.size() - 1).lines.size(); i++)
		{
			Chao::CSD::CProject::DestroyScene(rcDeco.Get(), DecoInfoCache.at(DecoInfoCache.size() - 1).lines.at(i));			
		}
	}
	DecoInfoCache.clear();
	rcDeco = nullptr;
	static boost::shared_ptr<hh::db::CRawData> rawData;
	auto database = hh::db::CDatabase::CreateDatabase();
	auto& databaseLoader = Sonic::CApplicationDocument::GetInstance()->m_pMember->m_spDatabaseLoader;

	databaseLoader->CreateArchiveList("DecoTextManager.ar", "DecoTextManager.arl", { 0, 0 });
	databaseLoader->LoadArchiveList(database, "DecoTextManager.arl");
	databaseLoader->LoadArchive(database, "DecoTextManager.ar", { 0, 0 }, false, false);
	Sonic::CCsdDatabaseWrapper wrapper(database.get());
	auto spCsdProject = wrapper.GetCsdProject("ui_deco_alt");
	rcDeco = spCsdProject->m_rcProject;

}
void DecorationTextManager::CreateScreen(Sonic::CGameObject* pParentGameObject)
{
	if (rcDeco && !spDeco)
	{
		pParentGameObject->m_pMember->m_pGameDocument->AddGameObject(spDeco = boost::make_shared<Sonic::CGameObjectCSD>(rcDeco, 0.5f, "HUD", false), "main", pParentGameObject);
	}
}
void DecorationTextManager::KillScreen()
{
	if (spDeco)
	{
		spDeco->SendMessage(spDeco->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
		spDeco = nullptr;
	}
}
void DecorationTextManager::ToggleScreen(const bool visible, Sonic::CGameObject* pParentGameObject)
{
	if (visible)
		CreateScreen(pParentGameObject);
	else
		KillScreen();
}

DecoTextInfo DecorationTextManager::InitializeText(const char* text, float spacing, int alignment, Hedgehog::math::CVector position)
{
	
		static boost::shared_ptr<hh::db::CRawData> rawData;
		auto database = hh::db::CDatabase::CreateDatabase();
		auto& databaseLoader = Sonic::CApplicationDocument::GetInstance()->m_pMember->m_spDatabaseLoader;

		databaseLoader->CreateArchiveList("DecoTextManager.ar", "DecoTextManager.arl", { 0, 0 });
		databaseLoader->LoadArchiveList(database, "DecoTextManager.arl");
		databaseLoader->LoadArchive(database, "DecoTextManager.ar", { 0, 0 }, false, false);
		Sonic::CCsdDatabaseWrapper wrapper(database.get());
		auto spCsdProject = wrapper.GetCsdProject("ui_deco_alt");
		rcDeco = spCsdProject->m_rcProject;
	
	string input = string(text);
	std::stringstream ss(input);
	string line;
	vector<string> lines;
	while (getline(ss, line, '\n')) {
		lines.push_back(line);
	}
	if (lines.size() == 0)
		lines.push_back("");
	DecoInfoCache.push_back(DecoTextInfo(text));
	for (size_t i = 0; i < lines.size(); i++)
	{
		auto item = DecoInfoCache.at(DecoInfoCache.size()-1);
		auto uiLine = rcDeco->CreateScene("deco_text");
		uiLine->SetPosition(position.x(), position.y() + (spacing * i));
		uiLine->GetNode("Text_blue")->SetText(lines.at(i).c_str());
		item.lines.push_back(uiLine);
	}
	return DecoInfoCache.at(DecoInfoCache.size() - 1);	
}
//
//void ShowText(const char* text)
//{
//	string input = string(text);
//	std::stringstream ss(input);
//	string line;
//	vector<string> lines;
//	while (getline(ss, line, '\n')) {
//		lines.push_back(line);
//	}
//	if (lines.size() == 0)
//		lines.push_back("");
//	char buff[32];
//	for (size_t i = 0; i < 6; i++)
//	{
//		sprintf(buff, "text_line_%d", i);
//		if (i <= lines.size() - 1)
//		{
//			cts_guide_txt->GetNode(buff)->SetText(lines.at(i).c_str());
//		}
//		else
//		{
//			cts_guide_txt->GetNode(buff)->SetText(" ");
//		}
//	}
//}

void DecorationTextManager::Install()
{
	WRITE_MEMORY(0x016E11F4, void*, DecoTextRemoveCallback);
	
}