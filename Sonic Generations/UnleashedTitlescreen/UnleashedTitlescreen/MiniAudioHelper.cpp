struct AudioData {
    std::string cueName;
    SoLoud::Wav* source;
    unsigned int handle;
};
SoLoud::Soloud* soloud  ; // Create an instance of the SoLoud engine
std::vector<AudioData> wavs; // Create a vector to store multiple instances of WAV audio files
static std::vector<int> handles; // Create a vector to store multiple instances of WAV audio files

// Function to play a specific audio file
unsigned int MiniAudioHelper::PlayAudio(std::string cueName, const bool& sfxOrAudio, bool loop)
{
    for (size_t i = 0; i < wavs.size(); i++)
    {
        if (wavs.at(i).cueName == cueName)
            return 0;
    }
    if (cueName.rfind(".mp3") != cueName.size() - 4) {
        cueName.append(".mp3");
    }
    cueName.insert(0, "Audio/");
    std::ifstream file(cueName);
    if (!file)
    {
        printf(("\n[AudioHelper] File " + std::string(cueName) + " does not exist.").c_str());
        return 0;
    }
    try
    {

        SoLoud::Wav* newSource = new SoLoud::Wav;
        newSource->load(cueName.c_str()); // Load the audio file at the specified path
        newSource->setLooping(loop); // Set the looping flag for the audio file
        AudioData data = AudioData();
        data.source = newSource;
        data.cueName = cueName;
        data.handle = soloud->play(*newSource);
        wavs.push_back(data);
        return data.handle;
    }
    catch (const std::exception& ex)
    {
        return 0;
    }
}

void MiniAudioHelper::StopAudio(unsigned int& index)
{
    for (size_t i = 0; i < wavs.size(); i++)
    {
        if(wavs.at(i).handle == index)
            soloud->stopAudioSource(*wavs.at(i).source);
    } // Stop the audio file
    index = 0;
}
void MiniAudioHelper::Shutdown()
{
	printf("\nShutting down SoLoud...");
	soloud->deinit();
}
void MiniAudioHelper::Initialize() 
{
    soloud = new SoLoud::Soloud;
	soloud->init();
	//handle = MiniAudioHelper::PlayAudio("Peaceful_Strangers",0, true);
	
}