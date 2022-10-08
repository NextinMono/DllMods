HOOK(void, __fastcall, InvalidBitset, 0x41C9C0, int a1, int a2)
{
	MessageBoxExA(0, "Invalid Bitset<N> Position! Out-Of-Range [0x41C9C0]", NULL, MB_OK, 0);
	return originalInvalidBitset(a1, a2);

}
void ErrorMessages::Install() {
	INSTALL_HOOK(InvalidBitset);
}