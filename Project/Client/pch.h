#pragma once


//#include <VLD/vld.h>

#include "framework.h"
#include <crtdbg.h>

// ���� ���̺귯�� ��Ŭ���
#include <Engine/global.h>
#include <Engine/CEngine.h>

#ifndef _DEBUG
#pragma comment(lib, "Engine//Engine")
#else
#pragma comment(lib, "Engine//Engine_d")
#endif

// ��ũ��Ʈ ���̺귯�� ��Ŭ���
#ifndef _DEBUG
#pragma comment(lib, "Script//Script")
#else
#pragma comment(lib, "Script//Script_d")
#endif