#include "NtPCH.h"
#ifdef NT_WINDOWED_APP
	#include "examples/imgui_impl_dx12.cpp"
	#include "examples/imgui_impl_win32.cpp"
#else
	#define IMGUI_IMPL_OPENGL_LOADER_GLAD
	#include "examples/imgui_impl_opengl3.cpp"
	#include "examples/imgui_impl_glfw.cpp"
#endif // NT_WINDOWED_APP


