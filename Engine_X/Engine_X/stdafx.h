// stdafx.h

#pragma once

#include "targetver.h"

#define NOMINMAX

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// directx headers and libs
#include <d3d10.h>
#include <dxgi.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#pragma comment(lib,"d3d10.lib")
#include <D3Dcompiler.h>
#pragma comment(lib,"d3dcompiler.lib")

// our global includes goes down under
#include "uPrereq.h"
#include "uAllocator.h"
#include "uSystem.h"
#include "uMath.h"
#include "uBitmap.h"

#include "RenderSystemPrereq.h"
#include "SceneManagerPrereq.h"
