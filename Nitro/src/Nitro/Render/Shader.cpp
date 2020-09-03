#include "NtPCH.h"
#include "Shader.h"
#include "Renderer.h"

#include "Platform/Independent/OpenGL/OpenGLShader.h"
#include "Platform/Windows/D3D12/D3D12Shader.h"
#include <glad/glad.h>

namespace Nitro
{
	namespace Graphics
	{
		u32 ShaderParamTypeToOpenGLBaseType(const ShaderParamType& type)
		{
			switch (type)
			{
			case ShaderParamType::Bool:
				return GL_BOOL;
			case ShaderParamType::Float:
				return GL_FLOAT;
			case ShaderParamType::Float2:
				return GL_FLOAT;
			case ShaderParamType::Float3:
				return GL_FLOAT;
			case ShaderParamType::Float4:
				return GL_FLOAT;
			case ShaderParamType::Int:
				return GL_INT;
			case ShaderParamType::Int2:
				return GL_INT;
			case ShaderParamType::Int3:
				return GL_INT;
			case ShaderParamType::Int4:
				return GL_INT;
			case ShaderParamType::Mat3:
				return GL_FLOAT;
			case ShaderParamType::Mat4:
				return GL_FLOAT;
			}
			NT_ASSERT(false, "unknown type for shader parameter");
			return 0;
		}

		u32 ShaderParamTypeSize(ShaderParamType type)
		{
			switch (type)
			{
			case ShaderParamType::Bool:
				return 1;
			case ShaderParamType::Float:
				return 4;
			case ShaderParamType::Float2:
				return 4 * 2;
			case ShaderParamType::Float3:
				return 4 * 3;
			case ShaderParamType::Float4:
				return 4 * 4;
			case ShaderParamType::Int:
				return 4;
			case ShaderParamType::Int2:
				return 4 * 2;
			case ShaderParamType::Int3:
				return 4 * 3;
			case ShaderParamType::Int4:
				return 4 * 4;
			case ShaderParamType::Mat3:
				return 4 * 3 * 3;
			case ShaderParamType::Mat4:
				return 4 * 4 * 4;
			}
			NT_ASSERT(false, "unknown type for shader parameter");
			return 0;
		}

		Ref<Shader> Shader::Create(const char* filePath)
		{
			switch (Renderer::GetAPI())
			{
			case RendererAPI::API::OpenGL:
				return Nitro::CreateRef<gl::shader>(filePath);
			case RendererAPI::API::D3D12:
				return Nitro::CreateRef<dx::D3D12Shader>(filePath);
			}

			NT_ASSERT(false, "Unknown RendererAPI.");
			return nullptr;
		}

		Ref<Shader> Shader::Create(const hashkey_t& existing_key, const char* vs_data, const char* fs_data)
		{
			switch (Renderer::GetAPI())
			{
			case RendererAPI::API::OpenGL:
				return Nitro::CreateRef<gl::shader>(existing_key, vs_data, fs_data);
			case RendererAPI::API::D3D12:
				return Nitro::CreateRef<dx::D3D12Shader>(existing_key, vs_data, fs_data);
			}

			NT_ASSERT(false, "Unknown RendererAPI.");
			return nullptr;
		}

		ShaderLib* ShaderLib::sm_Instance = nullptr;

		ShaderLib* ShaderLib::GetInstance()
		{
			if (ShaderLib::sm_Instance == nullptr)
			{
				ShaderLib::sm_Instance = new ShaderLib();
			}
			return ShaderLib::sm_Instance;
		}
	}
}