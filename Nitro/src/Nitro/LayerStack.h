#pragma once
#include "NtPCH.h"

namespace Nitro
{
	namespace Base
	{
		// Forward declaration
		class Layer;
		class NITRO_API LayerStack
		{
		public:
			LayerStack();
			~LayerStack();

			/// Implement begin() & end() so we can directly iterate layer stack.
			inline std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
			inline std::vector<Layer*>::iterator end() { return m_Layers.end(); }

			/// Diff between 2 Push Ops: from the head / end
			__forceinline void Push(Layer* layer) 
			{
				m_Layers.emplace(m_Layers.begin()+(m_LayerInsertIdx++), layer); 
			}
			__forceinline void PushOverlay(Layer* layer) 
			{ 
				m_Layers.emplace_back(layer); 
			}

			void Pop(Layer* pop_target);
			void PopOverlay(Layer* pop_target);

		private:
			/// Reason it's std::vector is we need to iteration/insertion on it.
			std::vector<Layer*> m_Layers;
			u32 m_LayerInsertIdx;
		};
	}
}