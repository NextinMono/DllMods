#pragma once

class LetterboxHelper
{
	
public:
	static Hedgehog::Math::CVector2* Resolution;
	static void Initialize(float resW, float resH);
	static void SetResolution(float resW, float resH);
	static void ScaleScene(Chao::CSD::CScene* scene);
};
