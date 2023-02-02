HOOK(void, __fastcall, CHudSonicStageUpdate, 0x1098A50, void* thisDeclaration, void* edx, float* pUpdateInfo)
{
	// Force disable extended boost.
	*(uint32_t*)((uint32_t)*CONTEXT->ms_pInstance + 0x680) = 1;

	// Always clamp boost to 100.
	CONTEXT->m_ChaosEnergy = min(CONTEXT->m_ChaosEnergy, 0);

	originalCHudSonicStageUpdate(thisDeclaration, edx, pUpdateInfo);
}

void evSonic::Install()
{
	INSTALL_HOOK(CHudSonicStageUpdate);
}