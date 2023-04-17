Hedgehog::Math::CVector2* LetterboxHelper::Resolution;

HOOK(double, __fastcall, GetWidth, 0x675420)
{
	return LetterboxHelper::Resolution->x();
}
HOOK(double, __fastcall, GetHeight, 0x675430)
{
	return LetterboxHelper::Resolution->y();
}
HOOK(int, __cdecl, TestSpec, 0x682770, DWORD a1, int type, int a3, byte* a4, float a5, float offset)
{
	printf("\n\n\n");
	printf("\nA1: %lu", a1);
	printf("\nType: %d", type);
	printf("\nA3: %d", a3);
	printf("\nA5: %f", a5);
	printf("\nOffset: %f", offset);
	return originalTestSpec(a1, 1, a3, a4, a5, offset);
}
void LetterboxHelper::SetResolution(float resW = 1280, float resH = 720)
{
	Resolution = new Hedgehog::Math::CVector2(resW, resH);
}
void LetterboxHelper::ScaleScene(Chao::CSD::CScene* scene)
{
	/*printf("\n\n%f", 1.0f / (4.0f / 3.0f));
	scene->SetScale(1.0f / (4.0f / 3.0f), 1);*/
}
void LetterboxHelper::Initialize(float resW = 1280, float resH = 720)
{
	Resolution = new Hedgehog::Math::CVector2(resW, resH);
	//INSTALL_HOOK(GetWidth);/*
	//INSTALL_HOOK(TestSpec);*/
	//INSTALL_HOOK(GetHeight);
}