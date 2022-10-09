#pragma once

#define WIN32_LEAN_AND_MEAN

#include <BlueBlur.h>
#include "INIReader.h"
#include "ModLoader.h"
#include "StringHelper.h"
#include "Helpers.h"
#include "Common.h"
#include "ScoreGenerationsAPI.h"

#include <cstdint>
#include <cstdio>
#include <string>
#include <filesystem>
#include <memory>
#include <windows.h>
#include <winuser.h>
#include <fstream>
#define C_RED     "\x1b[31m"
#define C_GREEN   "\x1b[32m"
#define C_YELLOW  "\x1b[33m"
#define C_BLUE    "\x1b[34m"
#define C_MAGENTA "\x1b[35m"
#define C_CYAN    "\x1b[36m"
#define C_RESET   "\x1b[0m"

#include "ErrorMessageExplainer.h"

#include "detours\include\detours.h"

#include "HudBtnGuide.h"
#include "Test.h"
#include "CSDCommon.h"
#include "Title.h"
#include "Configuration.h"
#include "HudLoading.h"
#include "HudResult.h"
#include "HudSonicStage.h"
#include "HudPause.h"
#include "HudGate.h"
#include "TestingCode.h"
#include "SubtitleOmochao.h"

#include "ArchivePatcher.h"