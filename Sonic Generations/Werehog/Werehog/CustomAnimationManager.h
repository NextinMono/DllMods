#pragma once
struct CAnimationStateInfo
{
	const char* name;
	const char* fileName;
	float speed;
	int32_t playbackType;
	int32_t field10;
	float field14;
	float field18;
	int32_t field1C;
	int32_t field20;
	int32_t field24;
	int32_t field28;
	int32_t field2C;
};

struct AnimationEntry
{
	std::string stateName;
	std::string fileName;
	
};
struct CAnimationStateSet
{
	CAnimationStateInfo* entries;
	size_t count;
};

class CustomAnimationManager
{

public:
	static void Install();
	static void RegisterAnimation(std::string stateName, std::string fileName);
	static std::vector<AnimationEntry> GetAnims();
};