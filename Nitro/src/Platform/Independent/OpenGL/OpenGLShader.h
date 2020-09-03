#pragma once

#include "Nitro/Render/Shader.h"

namespace Nitro
{
	namespace Graphics
	{
		namespace gl
		{
			class shader : public Shader
			{
				friend class vtxarray;
				typedef std::unordered_map<u32, std::string> shader_type2src_map;
			
			public:
				static const char* VERTEX_SHADER_IDENTIFIER;
				static const char* FRAGMENT_SHADER_IDENTIFIER;

			public:
				static shader_type2src_map preprocess_filetext(const std::string& shaderText);
				static u32 shadertype_atoi(const std::string& shadertype_name);

			public:
				shader(const char* filePath);
				shader(const hashkey_t& existing_key, const char* vs_data, const char* fs_data);
				virtual ~shader();

#pragma region Interface Methods
			public:
				virtual void Bind() const override;
				virtual void Unbind() const override;

				virtual void SetFloat(const char* name, float val) override;
				virtual void SetFloat2(const char* name, const glm::vec2& val) override;
				virtual void SetFloat3(const char* name, const glm::vec3& val) override;
				virtual void SetFloat4(const char* name, const glm::vec4& val) override;

				virtual void SetMat3(const char* name, const glm::mat3& val) override;
				virtual void SetMat4(const char* name, const glm::mat4& val) override;

				virtual const hashkey_t GetUniqueKey() const override;
#pragma endregion
			private:
				u32 compile(const shader_type2src_map& src_data);
				void upload_int(const std::string& name, int value);
				void upload_intArray(const std::string& name, int* values, uint32_t count);

				void upload_float(const std::string& name, float value);
				void upload_float2(const std::string& name, const glm::vec2& value);
				void upload_float3(const std::string& name, const glm::vec3& value);
				void upload_float4(const std::string& name, const glm::vec4& value);

				void upload_mat3(const std::string& name, const glm::mat3& matrix);
				void upload_mat4(const std::string& name, const glm::mat4& matrix);
			private:
				hashkey_t	gid;	// used for ShaderLib as index, hashed from name
				u32			rid;
			};
		}
	}
}