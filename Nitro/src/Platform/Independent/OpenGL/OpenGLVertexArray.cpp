#include "NtPCH.h"
#include "OpenGLVertexArray.h"
#include "Platform/Independent/OpenGL/OpenGLBuffer.h"
#include "Platform/Independent/OpenGL/OpenGLShader.h"

namespace Nitro
{ 
	namespace Graphics
	{
		namespace gl
		{
			vtxarray::vtxarray()
			{
				glCreateVertexArrays(1, &rid);
			}
			vtxarray::~vtxarray()
			{
				glDeleteVertexArrays(1, &rid);
			}
			void vtxarray::Bind() const
			{
				glBindVertexArray(rid);
			}
			void vtxarray::Unbind() const
			{
				glBindVertexArray(0);
			}
			void vtxarray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vbo)
			{
				NT_CORE_ASSERT(vbo->GetLayout().Stride != 0, "vbo doesn't have a BufferLayout setup.");
				
				this->Bind();
				
				vbo->Bind();
				const BufferLayout& layout = vbo->GetLayout();
				u32 attr_idx = 0;
				for (const BufferElement& elem : layout)
				{
					glEnableVertexAttribArray(attr_idx);
					glVertexAttribPointer(
						attr_idx,
						elem.GetComponentCountByType(),
						ShaderParamTypeToOpenGLBaseType(elem.Type),
						elem.Normalized ? GL_TRUE : GL_FALSE,
						layout.Stride,
						(const void*)elem.Offset
					);
					attr_idx++;
				}

				vtxbuffers.push_back(vbo);
			}
			void vtxarray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& ibo)
			{
				this->Bind();
				ibo->Bind();
				idxbuffer = ibo;
			}
			const std::vector<std::weak_ptr<VertexBuffer>>& vtxarray::GetVertexBuffers() const
			{
				// TODO: insert return statement here
				return vtxbuffers;
			}
			const std::weak_ptr<IndexBuffer>& vtxarray::GetIndexBuffer() const
			{
				return idxbuffer;
			}
		}
	}
}