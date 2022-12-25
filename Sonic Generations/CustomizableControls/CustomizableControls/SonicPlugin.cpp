static uint32_t pressedARes = 1;

HOOK(int*, __fastcall, NextGenSonic_CSonicStatePluginBoostBegin, 0x1117A20, void* This)
{
    Sonic::SPadState const* padState = &Sonic::CInputState::GetInstance()->GetPadState();
    if (padState->IsDown(/*(Sonic::EKeyState)Configuration::BoostButton*/Sonic::eKeyState_RightTrigger))
    {
        return originalNextGenSonic_CSonicStatePluginBoostBegin(This);
    }
}


bool __fastcall NextGenSonic_CanActivateEliseShield()
{
    float* currentBoost = Common::GetPlayerBoost();
    Sonic::SPadState const* padState = &Sonic::CInputState::GetInstance()->GetPadState();

    return
        *currentBoost > 0.0f &&
        padState->IsDown(/*(Sonic::EKeyState)Configuration::BoostButton*/Sonic::eKeyState_RightTrigger);
}

HOOK(bool, __fastcall, DoXButtonInput, 0xDFF120, CSonicContext* This, int a2)
{
    float* currentBoost = Common::GetPlayerBoost();
    Sonic::SPadState const* padState = &Sonic::CInputState::GetInstance()->GetPadState();

    return
        /**currentBoost > 0.0f &&*/
        padState->IsDown(/*(Sonic::EKeyState)Configuration::BoostButton*/Sonic::eKeyState_RightTrigger);
}
HOOK(bool, __fastcall, DoXButtonInput2, 0xDFFEF0, CSonicContext* This, int a2)
{
    float* currentBoost = Common::GetPlayerBoost();
    Sonic::SPadState const* padState = &Sonic::CInputState::GetInstance()->GetPadState();

    return
        /**currentBoost > 0.0f &&*/
        padState->IsDown(/*(Sonic::EKeyState)Configuration::BoostButton*/Sonic::eKeyState_RightTrigger);
}
HOOK(char, __stdcall, Trick, 0xE4AE30, void* a1)
{
    float* currentBoost = Common::GetPlayerBoost();
    Sonic::SPadState const* padState = &Sonic::CInputState::GetInstance()->GetPadState();

    bool returned = padState->IsDown(Sonic::eKeyState_A);
    return !returned;
}
void __declspec(naked) NextGenSonic_CSonicStatePluginBoostAdvance()
{
    static uint32_t returnAddress = 0x1117731;
    static uint32_t successAddress = 0x11178F1;
    __asm
    {
        push    eax
        push    ecx
        call    NextGenSonic_CanActivateEliseShield
        xor al, 1 // flip it so we can exit for false
        mov     byte ptr[ebp + 1Ch], al
        pop     ecx
        pop     eax

        // Use normal advance for Super Sonic
        cmp     byte ptr[ecx + 6Fh], 0
        jnz     jump
        jmp[successAddress]

        // original function
        jump:
        mov     byte ptr[ebp + 1Ch], 0
            mov[esp + 0Fh], 0
            jmp[returnAddress]
    }
}
bool isPressedA()
{
    Sonic::SPadState const* padState = &Sonic::CInputState::GetInstance()->GetPadState();
    
    return
        /**currentBoost > 0.0f &&*/
        padState->IsDown(/*(Sonic::EKeyState)Configuration::BoostButton*/Sonic::eKeyState_A);

}
int isPressedAInt()
{
    bool e = isPressedA();
    pressedARes = e;
    if(pressedARes == 1)
        DebugBreak();
    return pressedARes;

}

void __declspec(naked) SetBoostButton()
{
    static uint32_t pAddr = 0x00DFF268;
    static uint32_t addrIf0 = 0x00DFF2CB;
    __asm
    {
        call isPressedA
        test al, al
        jz jump
        jmp[pAddr]
       
        jump:
        jmp[addrIf0]
    }
}
void __declspec(naked) TitleUI_SetCutsceneTimer()
{
    static uint32_t pAddr = 0x00E4AE6F;
    static uint32_t addrIf0 = 0x00E4AEAB;
    __asm
    {
        call isPressedAInt
        test al, al
        
        movss xmm0, al
            jmp[pAddr]
   

        

    }
}
void __declspec(naked) hhhhh()
{
    static uint32_t pAddr = 0x00E4AEA0;
    static uint32_t addrIf0 = 0x00E4AEAB;
    __asm
    {
        call isPressedAInt
        cmp pressedARes, 0
        jg greater
        jmp[addrIf0]

        greater:
        retn 0
    }
}

void SonicPlugin::Install()
{
   /* WRITE_MEMORY(0xDFF268, uint8_t, 0xF3, 0x0F, 0x10, 0x83, 0xBC);
    WRITE_MEMORY(0xDFE05F, uint8_t, 0xF3, 0x0F, 0x10, 0x86, 0xBC);*/
    INSTALL_HOOK(DoXButtonInput);
    INSTALL_HOOK(DoXButtonInput2);
   //INSTALL_HOOK(Trick);
    //WRITE_JUMP(0x1117728, NextGenSonic_CSonicStatePluginBoostAdvance);
    WRITE_JUMP(0x00DFF254, SetBoostButton);
    WRITE_JUMP(0x00E4AE6A, TitleUI_SetCutsceneTimer);
    WRITE_JUMP(0x00E4AE65, hhhhh);
}