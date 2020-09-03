#pragma once
#include <d3d12.h>

namespace Nitro
{
	namespace Graphics
	{
		namespace dx
		{
			namespace common
			{
				extern D3D12_DEPTH_STENCIL_DESC DepthStencilState_Disabled;
				extern D3D12_DEPTH_STENCIL_DESC DepthStencilState_ReadWrite_GE;
				extern D3D12_DEPTH_STENCIL_DESC DepthStencilState_ReadWrite_LT;
				extern D3D12_DEPTH_STENCIL_DESC DepthStencilState_ReadOnly_GE;
				extern D3D12_DEPTH_STENCIL_DESC DepthStencilState_ReadOnly_LT;
				extern D3D12_DEPTH_STENCIL_DESC DepthStencilState_ReadOnly_EQ;

				extern D3D12_RASTERIZER_DESC RasterizerDefault_CCW_CullBack;    // Counter-clockwise
				extern D3D12_RASTERIZER_DESC RasterizerDefault_CW_CullBack;
				extern D3D12_RASTERIZER_DESC RasterizerDefault_CCW_TwoSided;

				extern D3D12_BLEND_DESC BlendState_NoColorWrite;
				extern D3D12_BLEND_DESC BlendState_Disable;
				extern D3D12_BLEND_DESC BlendState_PreMultiplied;
				extern D3D12_BLEND_DESC BlendState_Traditional;
				extern D3D12_BLEND_DESC BlendState_Additive;
				extern D3D12_BLEND_DESC BlendState_TraditionalAdditive;

			}

			extern void InitializeCommonStateVars();
		}
	}
}