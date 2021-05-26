#pragma once

#define WIN32_LEAN_AND_MEAN
#define NO_MIN_MAX

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>

#ifdef TEXRP_BUILD
using namespace Microsoft::WRL;
#endif
