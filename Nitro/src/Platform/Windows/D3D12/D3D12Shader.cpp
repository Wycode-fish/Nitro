#include "NtPCH.h"
#include "D3D12Shader.h"

namespace Nitro
{
	namespace Graphics
	{
#ifdef NT_WINDOWED_APP
#endif
		namespace dx
		{
			std::string D3D12Shader::g_ShaderModelVersion = "_5_1";

			std::string D3D12Shader::GetTargetType(const D3D12ShaderType& type)
			{
				switch (type)
				{
				case D3D12ShaderType::VS:
					return "vs" + D3D12Shader::g_ShaderModelVersion;
				case D3D12ShaderType::PS:
					return "ps" + D3D12Shader::g_ShaderModelVersion;
				}
				NT_ASSERT(false, "unsupported shader type.");
				return std::string();
			}

			D3D12Shader::D3D12Shader(const char* filePath)
				: m_Data{nullptr, nullptr}
			{
			}
			D3D12Shader::D3D12Shader(const hashkey_t& existing_key, const char* vs_data, const char* fs_data)
				: m_Data{ nullptr, nullptr }
			{
			}
			D3D12Shader::~D3D12Shader()
			{
			}
			void D3D12Shader::Bind() const
			{
			}
			void D3D12Shader::Unbind() const
			{
			}
			void D3D12Shader::SetFloat(const char* name, float val)
			{
			}
			void D3D12Shader::SetFloat2(const char* name, const glm::vec2& val)
			{
			}
			void D3D12Shader::SetFloat3(const char* name, const glm::vec3& val)
			{
			}
			void D3D12Shader::SetFloat4(const char* name, const glm::vec4& val)
			{
			}
			void D3D12Shader::SetMat3(const char* name, const glm::mat3& val)
			{
			}
			void D3D12Shader::SetMat4(const char* name, const glm::mat4& val)
			{
			}
			const hashkey_t D3D12Shader::GetUniqueKey() const
			{
				return this->m_ID;
			}
			u32 D3D12Shader::Compile(const std::string& fileName, D3D12ShaderType type, ID3DBlob** buffer_byteCode, ID3DBlob** buffer_error) const
			{
				std::wstring fname = NT_STDSTR_TO_STDWSTR(fileName);
				std::string target_type = D3D12Shader::GetTargetType(type);

				u32 hr = D3DCompileFromFile(
					fname.c_str(),
					/*macro*/nullptr, /*includes*/nullptr, /*entryPoint*/"main",
					target_type.c_str(),
					/*flag1*/D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, /*flag2*/0,
					buffer_byteCode,
					buffer_error
				);

				NT_ASSERT(SUCCEEDED(hr), "shader compile failed.");
				return hr;
			}
		}
	}
}