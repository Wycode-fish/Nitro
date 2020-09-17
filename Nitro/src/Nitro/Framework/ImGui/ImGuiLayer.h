#pragma once

#include "Nitro/Layer.h"

namespace Nitro
{
	using namespace Base;

	namespace Graphics
	{
		namespace dx
		{
			class D3D12DescriptorHeap_Static;
		}
	}
	namespace Framework
	{
		class NITRO_API ImGuiLayer : public Layer
		{
#ifdef NT_WINDOWED_APP
			static Graphics::dx::D3D12DescriptorHeap_Static* sm_ImGuiSRVHeap;
#endif
		public:
			ImGuiLayer();
			virtual void OnAttach() override;
			virtual void OnDetach() override;
			virtual void OnImGuiRender() override;

			void Begin();
			void End();
		private:
			float m_Now;
		};
	}
}