HOOK(void, __fastcall, InvalidBitset, 0x41C9C0, int a1, int a2)
{
	MessageBoxExA(0, "Invalid Bitset<N> Position! Out-Of-Range [0x41C9C0]", NULL, MB_OK, 0);
	return originalInvalidBitset(a1, a2);

}
HOOK(int, __fastcall, SFDPrint, 0x7E27B0, char* a1)
{
	MessageBoxExA(0, a1, NULL, MB_OK, 0);
	return originalSFDPrint(a1);

}
HOOK(int, __cdecl, vsprintff, 0xA6A665, char* DstBuf, const char* Format, va_list ArgList, void* Edx)
{
	printf(DstBuf, Format, ArgList);
	return originalvsprintff(DstBuf, Format, ArgList, Edx);

}
void ErrorMessages::Install() {
	/*INSTALL_HOOK(InvalidBitset);
	INSTALL_HOOK(vsprintff);
	INSTALL_HOOK(SFDPrint);*/
}