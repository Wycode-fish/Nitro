#pragma once
#include "Nitro/Render/Shader.h"
#include <d3d12.h>

namespace Nitro
{
	namespace Graphics
	{
		namespace dx
		{
			class D3D12Shader : public Shader
			{
			public:
				enum class D3D12ShaderType
				{
					VS = 0, PS
				};
				static std::string g_ShaderModelVersion;
				static std::string GetTargetType(const D3D12ShaderType& type);
			public:
				D3D12Shader(const char* filePath);
				D3D12Shader(const hashkey_t& existing_key, const char* vs_data, const char* fs_data);
				virtual ~D3D12Shader();

				virtual void Bind() const override;
				virtual void Unbind() const override;

				virtual void SetFloat(const char* name, float val) override;
				virtual void SetFloat2(const char* name, const glm::vec2& val) override;
				virtual void SetFloat3(const char* name, const glm::vec3& val) override;
				virtual void SetFloat4(const char* name, const glm::vec4& val) override;

				virtual void SetMat3(const char* name, const glm::mat3& val) override;
				virtual void SetMat4(const char* name, const glm::mat4& val) override;

				virtual const hashkey_t GetUniqueKey() const override;
			public:
				u32 Compile(const std::string& fileName, D3D12ShaderType type, ID3DBlob** buffer_byteCode, ID3DBlob** buffer_error) const;
			private:
				std::array<ID3DBlob*, 2>	m_Data;
				hashkey_t					m_ID;
			};

			
		}
	}
}