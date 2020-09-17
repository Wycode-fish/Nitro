#pragma once

#ifdef NT_DEBUG_ASSERTS_ENABLED
#ifdef NT_WINDOWED_APP
#define NT_ASSERT(cond, ...) { if (!(cond)) { const std::wstring temp = NT_STDWSTR_FORMAT(__VA_ARGS__); MessageBox(0, temp.c_str(), L"Assert Failure", MB_OK); __debugbreak();}}
#define NT_CORE_ASSERT(cond, ...) NT_ASSERT(cond, __VA_ARGS__)
#define NT_MB_MSG(msg) { const std::wstring temp = NT_STDWSTR_FORMAT(msg); MessageBox(0, temp.c_str(), L"Message Break", MB_OK); __debugbreak(); }
#else
#define NT_ASSERT(x, ...) { if (!(x)) { NT_ERROR("Assert FAILED: {0}", __VA_ARGS__); __debugbreak();}}
#define NT_CORE_ASSERT(x, ...) { if (!x) { NT_CORE_ERROR("Assert FAILED: {0}", __VA_ARGS__); __debugbreak();}}
#endif
#else
#define NT_ASSERT(x, ...)
#define NT_CORE_ASSERT(x, ...)
#endif	// DEBUG_ASSERT