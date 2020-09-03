#pragma once

namespace Nitro
{
	namespace Graphics
	{
		// @ Interface for rendering contexts
		class IRenderingContext
		{
		public:
			virtual void Init() = 0;
			virtual void SwapBuffers() = 0;

		protected:
			virtual void LogCtxSpecs() = 0;
		};

	}
}