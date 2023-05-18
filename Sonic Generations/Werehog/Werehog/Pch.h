#define WIN32_LEAN_AND_MEAN

#include <BlueBlur.h>
#include "INIReader.h"
#include "ModLoader.h"
#include "StringHelper.h"
#include "Helpers.h"
#include "Common.h"
#include "ScoreGenerationsAPI.h"
#include "detours\include\detours.h"
#include <fstream>
#include <cstdint>
#include <cstdio>
#include <string>
#include <filesystem>
#include <memory>
#include <windows.h>
#include <winuser.h>
#include <fstream>
#include <algorithm>	
#include <vector>
#include <rapidxml/rapidxml.hpp>


#include <stdio.h>
#define DEBUG_DRAW_TEXT_DLL_IMPORT	
#include "DebugDrawText.h"	
#include "CustomAnimationManager.h"	

#include "ArchiveTreePatcher.h"
#include "Configuration.h"
#include "XMLParser.h"
#include "Sonic.h"