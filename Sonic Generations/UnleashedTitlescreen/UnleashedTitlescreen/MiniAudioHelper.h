#pragma once
struct AudioHandle {
	SharedPtrTypeless handleCri;
	unsigned int handleSoloud;
};
class MiniAudioHelper
{

public:
	static void Initialize();
	static void Shutdown();
	static void StopAudio(unsigned int& index);
	static unsigned int PlayAudio(std::string cueName, const bool& sfxOrAudio, bool loop);
	static void PlaySound(AudioHandle& handle, int cueCri, std::string cueSoloud, bool loop = false, bool forceSoloud = false);
	static void StopSound(AudioHandle& handle, bool forceSoloud = false);
};
