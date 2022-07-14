#pragma once
class HudLoading
{
public:
	enum LoadingArchiveType : int
	{
		LAT_Gens,
		LAT_UP,
		LAT_Colors,
		LAT_Custom
	};
	struct StageData
	{
		size_t m_stage;
		std::string m_stageID;

		bool m_disableSilverMedal;
		bool m_isBoss;
		bool m_isMission;

		char m_bestTime[16];
		uint32_t m_bestScore;
		uint32_t m_bestRing;
		uint32_t m_silverMedalCount;

		StageData()
		{
			m_stage = -1;
			m_stageID = "";

			m_isBoss = false;

			sprintf(m_bestTime, "");
			m_bestScore = 0;
			m_bestRing = 0;
			m_silverMedalCount = 0;
		}
	};

	static void Install();

	static void StartFadeOut();
	static bool IsFadeOutCompleted();
	static void StartResidentLoading();
	static void EndResidentLoading();
};