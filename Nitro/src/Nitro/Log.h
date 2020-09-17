#pragma once

#include "spdlog/fmt/ostr.h"
#include "spdlog/spdlog.h"

namespace Nitro
{
	class NITRO_API Log
	{
	public:
		static void Init();
		static std::shared_ptr<spdlog::logger> GetCoreLogger() { return s_CoreLogger; }
		static std::shared_ptr<spdlog::logger> GetClientLogger() { return s_ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}
// core logger macro
#define NT_CORE_TRACE(...)	Nitro::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define NT_CORE_INFO(...)	Nitro::Log::GetCoreLogger()->info(__VA_ARGS__)
#define NT_CORE_WARN(...)	Nitro::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define NT_CORE_ERROR(...)	Nitro::Log::GetCoreLogger()->error(__VA_ARGS__)
#define NT_CORE_FATAL(...)	Nitro::Log::GetCoreLogger()->fatal(__VA_ARGS__)

// client logger macro
#define NT_TRACE(...)	Nitro::Log::GetClientLogger()->trace(__VA_ARGS__)
#define NT_INFO(...)	Nitro::Log::GetClientLogger()->info(__VA_ARGS__)
#define NT_WARN(...)	Nitro::Log::GetClientLogger()->warn(__VA_ARGS__)
#define NT_ERROR(...)	Nitro::Log::GetClientLogger()->error(__VA_ARGS__)
#define NT_FATAL(...)	Nitro::Log::GetClientLogger()->fatal(__VA_ARGS__)