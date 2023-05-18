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

enum ResourceType
{
	CSB,
	Effect
};
struct Param
{
	std::string FileName;
	std::string Cue;
};
struct Resource
{
	int ID;
	ResourceType Type;
	std::vector< Param> Params;

};
struct ResourceInfo
{
	std::vector< Resource> Resources;
};
struct WerehogAttackNew
{
	std::string ActionName;
	std::string MotionName;
	int ValidLevel_Min;
	int ValidLevel_Max;
	bool ValidCommon;
	bool ValidBerserker;
	std::string KEY__YDown;
	std::string KEY__XDown;
	std::string KEY__ADown;
	std::string KEY__Land;
	std::string KEY__AirCombo;
	std::string KEY__End;
	bool Guard;
	bool Avoid;
	float KEY__StartFrame;
	float KEY__EndFrame;
	float WaitEndMotionEndFrame;
	float WaitEndMotionSpeed;
	float EndMotionSpeed;
	float LandStartFrame;
	float ActionValidHeightMin;
	ResourceInfo ResourceInfos;

	//From the respective attacks tbres files

};
class evSonic
{

public:
	static void Install();


};

