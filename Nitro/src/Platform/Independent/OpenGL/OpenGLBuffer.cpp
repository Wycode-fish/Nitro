#include "NtPCH.h"
#include "OpenGLBuffer.h"

namespace Nitro
{
	namespace Graphics
	{
		namespace gl
		{
			// @ -------------------------------------------------------------
			// @						VERTEXBUFFER
			// @ -------------------------------------------------------------
			vtxbuffer::vtxbuffer(float* vboData, const size_t& dataSz)
			{
				glCreateBuffers(1, &rid);
				glBindBuffer(GL_ARRAY_BUFFER, rid);
				glBufferData(GL_ARRAY_BUFFER, dataSz, (void*)vboData, GL_STATIC_DRAW);
			}
			vtxbuffer::~vtxbuffer()
			{
				glDeleteBuffers(1, &rid);
			}
			void vtxbuffer::Unbind() const
			{
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}
			void vtxbuffer::Bind() const
			{
				glBindBuffer(GL_ARRAY_BUFFER, rid);
			}

			// @ -------------------------------------------------------------
			// @						INDEXBUFFER
			// @ -------------------------------------------------------------
			idxbuffer::idxbuffer(u32* iboData, const size_t& cnt)
				: idx_count(cnt)
			{
				glCreateBuffers(1, &rid);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rid);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, cnt * sizeof(u32), (void*)iboData, GL_STATIC_DRAW);
			}
			idxbuffer::~idxbuffer()
			{
				glDeleteBuffers(1, &rid);
			}
			void idxbuffer::Unbind() const
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			}
			void idxbuffer::Bind() const
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rid);
			}
		}
	}
}