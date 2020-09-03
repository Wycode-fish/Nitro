#pragma once

namespace Nitro
{
	namespace Graphics
	{
		enum class ShaderParamType
		{
			None = 0, Bool,
			Float, Float2, Float3, Float4,
			Int, Int2, Int3, Int4,
			Mat3, Mat4
		};

		enum ShaderType
		{
			Unknown = -1,
			VS, FS,
			TotalNum
		};
		// @ -------------------------------------------------------------
		// @				Shader-Specified Util Funcs
		// @ -------------------------------------------------------------
		extern u32 ShaderParamTypeToOpenGLBaseType(const ShaderParamType& type);
		extern u32 ShaderParamTypeSize(ShaderParamType type);

		class Shader
		{
		public:
			static Ref<Shader> Create(const char* filePath);
			static Ref<Shader> Create(const hashkey_t& existing_key, const char* vs_data, const char* fs_data);
		public:
			virtual ~Shader() = default;

			virtual void Bind() const = 0;
			virtual void Unbind() const = 0;

			virtual void SetFloat(const char* name, float val) = 0;
			virtual void SetFloat2(const char* name, const glm::vec2& val) = 0;
			virtual void SetFloat3(const char* name, const glm::vec3& val) = 0;
			virtual void SetFloat4(const char* name, const glm::vec4& val) = 0;
			
			virtual void SetMat3(const char* name, const glm::mat3& val) = 0;
			virtual void SetMat4(const char* name, const glm::mat4& val) = 0;
			
			virtual const hashkey_t GetUniqueKey() const = 0;
		};

		class ShaderLib
		{
			NON_COPYABLE(ShaderLib);
		public:
			static ShaderLib* GetInstance();
		private:
			static ShaderLib* sm_Instance;
			ShaderLib() = default;

		public:
			inline void Add(const hashkey_t& key, const Ref<Shader>& shader)
			{
				NT_ASSERT(!this->Exists(key), "Current add-in shader has been in shader library.");
				this->m_ShaderMap[key] = shader;
			}
			inline void Add(const Ref<Shader>& shader)
			{
				hashkey_t key = shader->GetUniqueKey();
				this->Add(key, shader);
			}
			inline Ref<Shader> Load(const char* filePath)
			{
				Ref<Shader> shader = Shader::Create(filePath);
				this->Add(shader);
				return shader;
			}
			inline Ref<Shader> Load(const hashkey_t& key, const char* filePath)
			{
				Ref<Shader> shader = Shader::Create(filePath);
				this->Add(key, shader);
				return shader;
			}

			inline bool Exists(const hashkey_t& key) const 
			{ 
				return this->m_ShaderMap.find(key) != this->m_ShaderMap.end(); 
			}

		private:
			std::unordered_map<hashkey_t, Ref<Shader>> m_ShaderMap;
		};
	}
}