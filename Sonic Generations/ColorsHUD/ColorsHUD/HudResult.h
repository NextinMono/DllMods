#pragma once

class HudResult
{
public:
	enum ResultState : int
	{
		Idle,
		MainWait,
		Main,
		NewRecord,
		Rank,
		Footer,
		FadeOut,
	};
	enum SideState : int
	{
		WaitingForTime,
		WaitingForRings,
		PlayEndSound,
		Finished
	}
	; enum ScoreState : int
	{
		ScoreIdle,
		ScoreRandomize
	};	
	enum ResultRankType : int
	{
		D,
		C,
		B,
		A,
		S
	};
	struct ResultData
	{
		int m_score;
		ResultRankType m_rank;
		ResultRankType m_perfectRank;
		int m_nextRankTime;
		float m_totalProp;	// result progress bar (time prop + ring prop) 
		float m_timeProp;	// result progress bar (time prop)
	};
	struct StageData
	{
		float m_ringScore;
		float m_speedScore;
		float m_enemyScore;
		float m_trickScore;

		StageData()
			: m_ringScore(0.0f)
			, m_speedScore(0.0f)
			, m_enemyScore(0.0f)
			, m_trickScore(0.0f)
		{}
	};
	static void Install();
};
