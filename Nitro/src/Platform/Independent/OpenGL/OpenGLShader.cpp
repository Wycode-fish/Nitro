#include "NtPCH.h"
#include "OpenGLShader.h"
#include "glad/glad.h"
#include "glm/gtc/type_ptr.hpp"

#include "Nitro/Util/FileOps.h"
#include "Nitro/Util/HashOps.h"

namespace Nitro
{
	namespace Graphics
	{
		namespace gl
		{
			const char* shader::VERTEX_SHADER_IDENTIFIER = "VERTEX";
			const char* shader::FRAGMENT_SHADER_IDENTIFIER = "FRAGMENT";
			
			u32 shader::shadertype_atoi(const std::string& shadertype_name)
			{
				if (shadertype_name == shader::FRAGMENT_SHADER_IDENTIFIER)
				{
					return GL_FRAGMENT_SHADER;
				}
				else if (shadertype_name == shader::VERTEX_SHADER_IDENTIFIER)
				{
					return GL_VERTEX_SHADER;
				}

				NT_ASSERT(false, "unknown type for shader parameter");
				return 0;
			}


			shader::shader_type2src_map shader::preprocess_filetext(const std::string& shaderText)
			{
				shader_type2src_map map;
				
				const char* type_macro = "#type";
				size_t start_pos = shaderText.find(type_macro, 0);
				
				while (start_pos != std::string::npos)
				{
					size_t eol = shaderText.find_first_of("\r\n", start_pos);
					NT_ASSERT(eol != std::string::npos, "Syntax error.");

					size_t typename_start = start_pos + 1 + strlen(type_macro);
					std::string shadertype_name = shaderText.substr(typename_start, eol - typename_start);
					
					NT_ASSERT(shader::shadertype_atoi(shadertype_name) != 0, "Invalid shader type.");

					size_t src_start = shaderText.find_first_not_of("\r\n", eol);
					NT_ASSERT(src_start != std::string::npos, "Syntax error.");
					size_t src_end = shaderText.find(type_macro, src_start);

					map[shader::shadertype_atoi(shadertype_name)] = (src_end == std::string::npos) ? shaderText.substr(src_start) : shaderText.substr(src_start, src_end - src_start);
					
					start_pos = src_end;
				}

				return map;
			}

			
			shader::shader(const char* filePath)
				: rid(0)
			{
				std::string shaderText = Nitro::Util::read_file2(filePath);
				shader_type2src_map type2src = shader::preprocess_filetext(shaderText);
				this->rid = this->compile(type2src);
				this->gid = Nitro::Util::hash_string(Nitro::Util::file_path2name(filePath));
			}

			shader::shader(const hashkey_t& existing_key, const char* vs_data, const char* fs_data)
				: gid(existing_key), rid(0)
			{
				shader_type2src_map type2src;
				type2src[shader::shadertype_atoi(shader::VERTEX_SHADER_IDENTIFIER)] = vs_data;
				type2src[shader::shadertype_atoi(shader::FRAGMENT_SHADER_IDENTIFIER)] = fs_data;
				this->rid = this->compile(type2src);
			}

			shader::~shader()
			{
				glDeleteProgram(rid);
			}

			void shader::Bind() const
			{
				glUseProgram(rid);
			}

			void shader::Unbind() const
			{
				glUseProgram(0);
			}

			void shader::SetFloat(const char* name, float val)
			{
				this->upload_float(name, val);
			}

			void shader::SetFloat2(const char* name, const glm::vec2& val)
			{
				this->upload_float2(name, val);
			}

			void shader::SetFloat3(const char* name, const glm::vec3& val)
			{
				this->upload_float3(name, val);
			}

			void shader::SetFloat4(const char* name, const glm::vec4& val)
			{
				this->upload_float4(name, val);
			}

			void shader::SetMat3(const char* name, const glm::mat3& val)
			{
				this->upload_mat3(name, val);
			}

			void shader::SetMat4(const char* name, const glm::mat4& val)
			{
				this->upload_mat4(name, val);
			}

			const hashkey_t shader::GetUniqueKey() const
			{
				return this->gid;
			}

			u32 shader::compile(const shader_type2src_map& src_data)
			{
				NT_ASSERT(src_data.size() <= 2, "Current total numbers of types of supported shaders is 2: VS / FS.");
				
				u32 programID = glCreateProgram();
				
				std::array<u32, 2> shaderIDs;
				u32 shaderIndex = 0;
				for (auto kvp : src_data)
				{
					u32 type = kvp.first;
					const char* src = kvp.second.c_str();

					u32 shader = glCreateShader(type);
					glShaderSource(shader, 1, &src, 0);
					glCompileShader(shader);

#ifdef NT_DEBUG
					i32 isCompiled;
					glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
					if (isCompiled == GL_FALSE)
					{
						int maxLogLength = 0;
						glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLogLength);

						std::vector<char> info(maxLogLength);
						glGetShaderInfoLog(shader, maxLogLength, &maxLogLength, &info[0]);
						NT_CORE_ERROR("Shader compile failed: {0}", info.data());
						glDeleteShader(shader);
						break; // no return yet, need to delete program first.
					}

#endif

					glAttachShader(programID, shader);
					shaderIDs[shaderIndex++] = shader;
				}

				glLinkProgram(programID);
#ifdef NT_DEBUG
				int isLinked = 0;
				glGetProgramiv(programID, GL_LINK_STATUS, (int*)&isLinked);
				if (isLinked == GL_FALSE)
				{
					GLint maxLength = 0;
					glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &maxLength);

					// The maxLength includes the NULL character
					std::vector<GLchar> infoLog(maxLength);
					glGetProgramInfoLog(programID, maxLength, &maxLength, &infoLog[0]);

					// We don't need the program anymore.
					glDeleteProgram(programID);

					for (u32 id : shaderIDs)
						glDeleteShader(id);

					NT_CORE_ERROR("Shader link failed: {0}", infoLog.data());

					return -1;
				}

#endif // NT_DEBUG

				for (u32 id : shaderIDs)
				{
					glDetachShader(programID, id);
					glDeleteShader(id);
				}

				return programID;
			}

			void shader::upload_int(const std::string& name, int value)
			{
				GLint location = glGetUniformLocation(rid, name.c_str());
				glUniform1i(location, value);
			}

			void shader::upload_intArray(const std::string& name, int* values, uint32_t count)
			{
				GLint location = glGetUniformLocation(rid, name.c_str());
				glUniform1iv(location, count, values);
			}

			void shader::upload_float(const std::string& name, float value)
			{
				GLint location = glGetUniformLocation(rid, name.c_str());
				glUniform1f(location, value);
			}

			void shader::upload_float2(const std::string& name, const glm::vec2& value)
			{
				GLint location = glGetUniformLocation(rid, name.c_str());
				glUniform2f(location, value.x, value.y);
			}

			void shader::upload_float3(const std::string& name, const glm::vec3& value)
			{
				GLint location = glGetUniformLocation(rid, name.c_str());
				glUniform3f(location, value.x, value.y, value.z);
			}

			void shader::upload_float4(const std::string& name, const glm::vec4& value)
			{
				GLint location = glGetUniformLocation(rid, name.c_str());
				glUniform4f(location, value.x, value.y, value.z, value.w);
			}

			void shader::upload_mat3(const std::string& name, const glm::mat3& matrix)
			{
				GLint location = glGetUniformLocation(rid, name.c_str());
				glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
			}

			void shader::upload_mat4(const std::string& name, const glm::mat4& matrix)
			{
				GLint location = glGetUniformLocation(rid, name.c_str());
				glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
			}
		}
	}
}