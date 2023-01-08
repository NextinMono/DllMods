#define WIN32_LEAN_AND_MEAN

#include <BlueBlur.h>
#include "INIReader.h"
#include "ModLoader.h"
#include "StringHelper.h"
#include "Helpers.h"
#include "Common.h"
#include "ScoreGenerationsAPI.h"
#include "ArchivePatcher.h"
#include "detours\include\detours.h"
#include <fstream>
#include <json/json.h>
#include <cstdint>
#include <cstdio>
#include <string>
#include <filesystem>
#include <memory>
#include <windows.h>
#include <winuser.h>
#include <fstream>

#define WITH_MINIAUDIO
#include <soloud.h>
#include <soloud_wav.h>

#include <stdio.h>
#define DEBUG_DRAW_TEXT_DLL_IMPORT	
#include "DebugDrawText.h"	
//
//constexpr float RAD2DEGf = 57.2958f;
//constexpr float DEG2RADf = 0.0174533f;


//

//
#include "MiniAudioHelper.h"
#include "Configuration.h"
#include "TitleWorldMap.h"
#include "LetterboxHelper.h"
#include "CSDCommon.h"
#include "Title.h"
#include "TitleOption.h"