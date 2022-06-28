/// <summary>
/// The main entry point for the injection.
/// </summary>
/// 
/// 
/// Mod code taken from ColorsHUD code by HyperBE32

HOOK(void, __fastcall, CHudSonicStageUpdate, 0x1098A50, void* thisDeclaration, void* edx, float* pUpdateInfo)
{
// Force disable extended boost.
*(uint32_t*)((uint32_t)*CONTEXT->ms_pInstance + 0x680) = 1;

// Always clamp boost to 100.
CONTEXT->m_ChaosEnergy = min(CONTEXT->m_ChaosEnergy, 100.0f);

originalCHudSonicStageUpdate(thisDeclaration, edx, pUpdateInfo);
}
extern "C" _declspec(dllexport) void Init()
{
	INSTALL_HOOK(CHudSonicStageUpdate);
}