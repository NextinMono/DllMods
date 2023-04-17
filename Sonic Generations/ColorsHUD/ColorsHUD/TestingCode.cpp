//call function: use this:
//inline FUNCTION_PTR(void, __fastcall, AdvanceTrickss, 0x52F390, Sonic::CGameObject* This, void* Edx, int a2); 
// replace the variables tho


HOOK(double, __fastcall, GetWidth, 0x675420)
{
	return 640;
}
HOOK(double, __fastcall, GetHeight, 0x675430)
{
	return 480;
}
HOOK(void, __fastcall, Test6, 0x1296DE0, void* Edx)
{
	printf("TRIG");
	originalTest6(Edx);
	WRITE_MEMORY(0x01E66B8C, char*, "ghz200");
}
void TestingCode::Install()
{
	//Test cast stretch
	WRITE_JUMP(0x006830BD, 0x006838EA);
#pragma region Resolution/Aspect Ratio Scaling
	//FUNCTION_PTR(void, __cdecl, TestSetAspect, 0x675440, int width, int height);
	//FUNCTION_PTR(void, __cdecl, SetResolution, 0x675400, float width, float height);
	///*INSTALL_HOOK(TestRestart);*/
	//	/*INSTALL_HOOK(TestRestart);*//*
	//WRITE_MEMORY(0x01B23F34, float, 4/3)*/
	///*WRITE_MEMORY(0x01703B30, float, 4);
	//WRITE_MEMORY(0x01703B28, float, 3);*/

	////Sets resolution of Viewport (640x480, DEF: 1280x720)
	///*WRITE_MEMORY(0x016D2E4C, float, 480);*//*
	//WRITE_MEMORY(0x01703B38, float, 640);*/
	//TestSetAspect(640, 480);
	//INSTALL_HOOK(GetWidth);
	//INSTALL_HOOK(GetHeight);

	////WRITE_MEMORY(0x01B23F34, float, (640/480) * 2);
	/*WRITE_MEMORY(0x01B23F00, float, 640);
	WRITE_MEMORY(0x01B23F04, float, 480);*/

	/*WRITE_MEMORY(0x01804F8C, float, 640);
	WRITE_MEMORY(0x01804F90, float, 480);*/
#pragma endregion
}