﻿#pragma once

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

#include "detours\include\detours.h"

#include "HudBtnGuide.h"
#include "CSDCommon.h"
#include "Title.h"
#include "Configuration.h"
#include "HudLoading.h"
#include "HudResult.h"
#include "HudSonicStage.h"
#include "HudPause.h"