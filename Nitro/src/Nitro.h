#pragma once

// This file's put inside of $(SolutionDir)Nitro/src instead of src/Nitro, since it's For client application to use.

/// Provided to client
#include "Nitro/Application.h"
#include "Nitro/Log.h"

#include "Nitro/Layer.h"
#include "Nitro/Framework/ImGui/ImGuiLayer.h"

#include "Nitro/Common/Configs/KeyCode.h"
#include "Nitro/Common/Configs/MouseButtonCode.h"

#include "Nitro/Common/Events/KeyEvent.h"
#include "Nitro/Common/Events/MouseEvent.h"
#include "Nitro/Common/Events/ApplicationEvent.h"
/// ---------- EntryPoint ----------
#include "Nitro/EntryPoint.h"
/// --------------------------------