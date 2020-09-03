#include "NtPCH.h"
#include "LayerStack.h"

namespace Nitro
{
	namespace Base
	{
		LayerStack::LayerStack()
			: m_LayerInsertIdx{ 0 }
		{}

		LayerStack::~LayerStack()
		{
			for (Layer* layer : m_Layers)
				if (layer)
					delete layer;
		}

		void LayerStack::Pop(Layer* pop_target)
		{
			std::vector<Layer*>::iterator pos = std::find(m_Layers.begin(), m_Layers.end(), pop_target);
			if (pos != m_Layers.end())
			{
				m_Layers.erase(pos);
				m_LayerInsertIdx--;
			}
		}

		void LayerStack::PopOverlay(Layer* pop_target)
		{
			std::vector<Layer*>::iterator pos = std::find(m_Layers.begin(), m_Layers.end(), pop_target);
			if (pos != m_Layers.end())
				m_Layers.erase(pos);
		}
	}
}