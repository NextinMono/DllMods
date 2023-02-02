BOOL WINAPI DllMain(_In_ HINSTANCE instance, _In_ DWORD reason, _In_ LPVOID reserved)
{
	switch (reason)
	{

	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH: 
	{
		MiniAudioHelper::Shutdown();
		break;
	}
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
		break;
	}

	return TRUE;
}
