#pragma once


//#include <VLD/vld.h>

#include "framework.h"
#include <crtdbg.h>

// 엔진 라이브러리 인클루드
#include <Engine/global.h>
#include <Engine/CEngine.h>

#ifndef _DEBUG
#pragma comment(lib, "Engine//Engine")
#else
#pragma comment(lib, "Engine//Engine_d")
#endif

// 스크립트 라이브러리 인클루드
#ifndef _DEBUG
#pragma comment(lib, "Script//Script")
#else
#pragma comment(lib, "Script//Script_d")
#endif