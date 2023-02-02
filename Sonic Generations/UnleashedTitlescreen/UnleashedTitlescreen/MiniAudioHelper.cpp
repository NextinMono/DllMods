struct AudioData {
    std::string cueName;
    SoLoud::Wav* source;
    unsigned int handle;
};
SoLoud::Soloud* soloud  ; // Create an instance of the SoLoud engine
std::vector<AudioData> wavs; // Create a vector to store multiple instances of WAV audio files
static std::vector<int> handles; // Create a vector to store multiple instances of WAV audio files

void LoadFile(SoLoud::Wav* source, std::string cueName)
{
    source->load(cueName.c_str()); // Load the audio file at the specified path
}
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
    cueName.insert(0, "Audio\\");
    cueName.insert(0, Configuration::modPath);
    std::ifstream file(cueName);
    if (!file)
    {
        printf(("\n[AudioHelper] File " + std::string(cueName) + " does not exist.").c_str());
        return 0;
    }
    try
    {

        SoLoud::Wav* newSource = new SoLoud::Wav;

        std::thread t1(LoadFile,newSource, cueName);
        t1.join(); // Waits for the new thread to finish before closing main thread
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

void MiniAudioHelper::PlaySound(AudioHandle& handle, int cueCri, std::string cueSoloud, bool loop, bool forceSoloud)
{
    if (!Configuration::CompatibilityMode && forceSoloud == false)
    {
        Common::PlaySoundStatic(handle.handleCri, cueCri);
    }
    else if(forceSoloud)
    {
        handle.handleSoloud = MiniAudioHelper::PlayAudio(cueSoloud, false, loop);
    }
}
void MiniAudioHelper::StopSound(AudioHandle& handle, bool forceSoloud)
{
    if (!Configuration::CompatibilityMode && forceSoloud == false)
    {
        handle.handleCri.reset();
    }
    else
    {
        MiniAudioHelper::StopAudio(handle.handleSoloud);
    }

}
void MiniAudioHelper::Initialize() 
{
    soloud = new SoLoud::Soloud;
	soloud->init();
	//handle = MiniAudioHelper::PlayAudio("Peaceful_Strangers",0, true);
	
}