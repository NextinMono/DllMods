#pragma once

class MiniAudioHelper
{

public:
	static void Initialize();
	static void Shutdown();
	static void StopAudio(unsigned int& index);
	static unsigned int PlayAudio(std::string cueName, const bool& sfxOrAudio, bool loop);
};
