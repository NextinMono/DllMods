#pragma once
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
struct Motion {
	std::string MotionName;
	std::string FileName;
	float MotionFirstSpeed;
	std::string ComposedFileName;
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
class XMLParser
{

public:
	static void Install();
	static std::vector<WerehogAttackNew> attacks;
	static std::vector<WerehogAttackNew> starterAttacks;
	static std::vector<Motion> animationTable;


};

