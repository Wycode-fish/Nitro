#pragma once
#include "Nitro/Core.h"
#include "Shader.h"

namespace Nitro
{
	namespace Graphics
	{
		// @ -------------------------------------------------------------
		// @						BUFFERLAYOUT
		// @ -------------------------------------------------------------
		struct BufferElement
		{
#ifdef NT_DEBUG
			std::string Name;
#endif // NT_DEBUG
			ShaderParamType Type;
			u32 Offset;
			bool Normalized;

			BufferElement() {}
			BufferElement(const std::string& name, ShaderParamType type, bool normalized)
				: Type(type)
				, Offset(0)
				, Normalized(normalized)
#ifdef NT_DEBUG
				, Name{ name }
#endif // NT_DEBUG
			{}

			u32 GetComponentCountByType() const
			{
				switch (this->Type)
				{
				case ShaderParamType::Bool:
					return 1;
				case ShaderParamType::Float:
					return 1;
				case ShaderParamType::Float2:
					return 2;
				case ShaderParamType::Float3:
					return 3;
				case ShaderParamType::Float4:
					return 4;
				case ShaderParamType::Int:
					return 1;
				case ShaderParamType::Int2:
					return 2;
				case ShaderParamType::Int3:
					return 3;
				case ShaderParamType::Int4:
					return 4;
				case ShaderParamType::Mat3:
					return 3 * 3;
				case ShaderParamType::Mat4:
					return 4 * 4;
				}
				NT_CORE_ERROR("Invalid shader param type.");
				return 0;
			}
		};
		struct BufferLayout
		{
			std::vector<BufferElement> Elements;
			u32 Stride;
			
			BufferLayout() {}
			BufferLayout(std::initializer_list<BufferElement> elements)
				: Elements(elements), Stride(0)
			{
				this->InitOffsetsForElements();
			}
			BufferLayout& operator=(const BufferLayout& other)
			{
				Elements = std::move(other.Elements);
				Stride = other.Stride;
				return *this;
			}
			std::vector<BufferElement>::iterator begin() { return Elements.begin(); }
			std::vector<BufferElement>::iterator end() { return Elements.end(); }
			std::vector<BufferElement>::const_iterator begin() const { return Elements.begin(); }
			std::vector<BufferElement>::const_iterator end() const { return Elements.end(); }
		private:
			void InitOffsetsForElements()
			{
				NT_CORE_ASSERT(Stride == 0, "Layout has been inited.");
				for (BufferElement elem : Elements)
				{
					elem.Offset = Stride;
					Stride += ShaderParamTypeSize(elem.Type);
				}
			}
		};

		// @ -------------------------------------------------------------
		// @						VERTEXBUFFER
		// @ -------------------------------------------------------------
		class VertexBuffer
		{
		public:
			static VertexBuffer* Create(float* data, const size_t& dataSz);
		public:
			virtual ~VertexBuffer() {}
			virtual void Bind() const = 0;
			virtual void Unbind() const = 0;
			virtual void SetLayout(const BufferLayout& layout) = 0;
			virtual const BufferLayout& GetLayout() const = 0;
			// ** why we can't put the BufferLayout as a member-var here?
			//
			//    answer: if there's any member-var inside, the derived class
			//			will ask for a default ctor from this base class
		};

		// @ -------------------------------------------------------------
		// @						INDEXBUFFER
		// @ -------------------------------------------------------------
		class IndexBuffer
		{
		public:
			static IndexBuffer* Create(u32* data, const size_t& cnt);
		public:
			virtual ~IndexBuffer() {}
			virtual void Bind() const = 0;
			virtual void Unbind() const = 0;
			virtual size_t GetCount() const = 0;
		};
	}
}