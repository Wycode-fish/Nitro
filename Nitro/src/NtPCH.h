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
#include "Nitro/Core.h"
#include "Nitro/Log.h"
#include "Nitro/Layer.h"
#include "Nitro/Common/Configs/KeyCode.h"
#include "Nitro/Common/Configs/MouseButtonCode.h"
#include "Nitro/OperatorNew.h"
#pragma endregion


#pragma region ===================== RSG =====================
#include "Nitro/Framework/rsg/String.h"
#include "Nitro/Framework/rsg/Queue.h"
#pragma endregion


#pragma region ===================== Debug =====================
#include "Nitro/Rttr/RttrBase.h"
#pragma endregion

#pragma region ===================== Debug =====================
#ifdef NT_DEBUG
#include "Nitro/Debug/LogMacro.h"
#include "Nitro/Debug/Trap.h"
#endif
#pragma endregion

#pragma region ===================== Utility =====================
#include "Nitro/Util/Delegate.h"
#include "Nitro/Util/AlgorithmOps.h"
#include "Nitro/Util/AlignOps.h"
#include "Nitro/Util/BaseOps.h"
#include "Nitro/Util/FileOps.h"
#include "Nitro/Util/HashOps.h"
#include "Nitro/Util/SimdOps.h"
#include "Nitro/Util/StringOps.h"
#pragma endregion

#pragma region ===================== Renderer =====================
#include "Nitro/Render/Renderer.h"
#include "Nitro/Render/RenderCommand.h"

#include "Nitro/Render/Buffer.h"
#include "Nitro/Render/Shader.h"
#include "Nitro/Render/VertexArray.h"

#include "Nitro/Render/OrthographicCamera.h"
#pragma endregion


