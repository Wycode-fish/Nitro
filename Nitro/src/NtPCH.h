#pragma once

#include <string>
#include <memory>
#include <functional>
#include <unordered_map>
#include <vector>
#include <queue>
#include <sstream>
#include <iostream>
#include <fstream>
#include <utility>

#ifdef NT_PLATFORM_WINDOWS
	#include <Windows.h>
	#include <d3d12.h>
	#include <dxgi1_6.h>
	#include <d3dcompiler.h>
	#include <DirectXMath.h>
	#include "Platform/Windows/D3D12/D3D12Util.h"
#else
#include <sys/time.h>
#endif

#pragma region ===================== Base =====================
#include "Nitro/Log.h"
#include "Nitro/Layer.h"
#include "Nitro/Common/Configs/KeyCode.h"
#include "Nitro/Common/Configs/MouseButtonCode.h"
#include "Nitro/Memory.h"
#include "Nitro/Utility.h"
#pragma endregion

#pragma region ===================== RSG =====================
#include "Nitro/Framework/rsg/Delegate.h"
#include "Nitro/Framework/rsg/String.h"
#include "Nitro/Framework/rsg/Memory.h"
#include "Nitro/Framework/rsg/Queue.h"
#pragma endregion

#pragma region ===================== Renderer =====================
#include "Nitro/Render/Renderer.h"
#include "Nitro/Render/RenderCommand.h"

#include "Nitro/Render/Buffer.h"
#include "Nitro/Render/Shader.h"
#include "Nitro/Render/VertexArray.h"

#include "Nitro/Render/OrthographicCamera.h"
#pragma endregion


