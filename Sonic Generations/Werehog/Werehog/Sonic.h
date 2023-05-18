#pragma once
enum AttackType {
Straight,
Hook,
Aerial,
Special
};
enum MoveType {
	Default,
	IngAutoTarget
};
struct WerehogAttack
{
	std::string comboName;
	std::vector <Sonic::EKeyState> combo;
	int cueIDs[5];

	std::vector<std::string> animations;
	const float duration[5];
	const float moveSpeed;
	AttackType attackType;

	//Parameters (some are from Unleashed)
	float motionBlendTime;
	float motionMoveSpeedRatio;
	bool dontJump;
	MoveType moveType;
};



class evSonic
{

public:
	static void Install();


};

