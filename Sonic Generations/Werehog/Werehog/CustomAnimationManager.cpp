std::vector<AnimationEntry> CUSTOM_ANIMATIONS
{
    {"EvilSonic_AttackCDHX", "evilsonic_attackCDHX"}
};


bool initialized;
//only here in case it ever changes
int GetAnimCount() { return CUSTOM_ANIMATIONS.size(); }
HOOK(void*, __cdecl, InitializeSonicAnimationList, 0x1272490)
{
    void* result = originalInitializeSonicAnimationList();
    {
        int animCount = GetAnimCount();
        CAnimationStateSet* set = (CAnimationStateSet*)0x15E8D40;
        CAnimationStateInfo* entries = new CAnimationStateInfo[set->count + 2 * animCount];

        std::copy(set->entries, set->entries + set->count, entries);

        for (size_t i = 0; i < 2 * animCount; i++)
        {
            const size_t animIndex = i % animCount;

            CAnimationStateInfo& entry = entries[set->count + i];

            entry.name = CUSTOM_ANIMATIONS[animIndex].stateName.c_str();
            entry.fileName = CUSTOM_ANIMATIONS[animIndex].fileName.c_str();
            entry.speed = 1.0f;
            entry.playbackType = 1;
            entry.field10 = 0;
            entry.field14 = -1.0f;
            entry.field18 = -1.0f;
            entry.field1C = 0;
            entry.field20 = -1;
            entry.field24 = -1;
            entry.field28 = -1;
            entry.field2C = -1;
        }

        WRITE_MEMORY(&set->entries, void*, entries);
        WRITE_MEMORY(&set->count, size_t, set->count + 2 * GetAnimCount());
    }
    initialized = true;
    return result;
}

HOOK(void, __fastcall, CSonicCreateAnimationStates, 0xE1B6C0, void* This, void* Edx, void* A2, void* A3)
{
    originalCSonicCreateAnimationStates(This, Edx, A2, A3);

    FUNCTION_PTR(void*, __stdcall, createAnimationState, 0xCDFA20,
        void* This, boost::shared_ptr<void>&spAnimationState, const hh::base::CSharedString & name, const hh::base::CSharedString & alsoName);

    for (size_t i = 0; i < 2 * GetAnimCount(); i++)
    {
        const size_t animIndex = i % GetAnimCount();
        const hh::base::CSharedString animName = CUSTOM_ANIMATIONS[animIndex].stateName.c_str();

        boost::shared_ptr<void> animationState;
        createAnimationState(A2, animationState, animName, animName);
    }
}
void CustomAnimationManager::RegisterAnimation(std::string stateName, std::string fileName)
{
    if (initialized)
        printf("\nYou cannot register animations after initialization!");
    else
    {
        AnimationEntry entry =  AnimationEntry();
        entry.fileName = fileName;
        entry.stateName = stateName;
        CUSTOM_ANIMATIONS.push_back(entry);
        printf(std::format("Registered animation ({0}, {1})", stateName, fileName).c_str());
    }
}

std::vector<AnimationEntry> CustomAnimationManager::GetAnims()
{
    return CUSTOM_ANIMATIONS;
}
void CustomAnimationManager::Install()
{
    // Add drift animations to the animation list
    INSTALL_HOOK(InitializeSonicAnimationList);
    INSTALL_HOOK(CSonicCreateAnimationStates);

}