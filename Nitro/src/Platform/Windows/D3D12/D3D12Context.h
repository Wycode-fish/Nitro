#pragma once

#include "Nitro/Render/IRenderingContext.h"

namespace Nitro
{
	namespace Graphics
	{
		class D3D12Context : public IRenderingContext
		{
		public:
			virtual void Init() override;
			virtual void SwapBuffers() override;

			// @ ----------------------------
			// @		Eason's attempt
			// @ ----------------------------
			virtual void Clear() const override;
		protected:
			virtual void LogCtxSpecs() override;
		};
	}
}