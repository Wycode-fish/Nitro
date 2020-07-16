#pragma once
#include "Nitro/Render/Shader.h"

namespace Nitro
{
	namespace Graphics
	{
		namespace dx
		{
			class shader : public Shader
			{
			public:
				shader(const std::string& shaderFile, u32 shaderType);
				virtual ~shader();
			};
		}
	}
}