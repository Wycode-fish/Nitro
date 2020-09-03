#include "NtPCH.h"
#include "D3D12CommonVars.h"

namespace Nitro
{
	namespace Graphics
	{
		namespace dx
		{
			namespace common
			{
				D3D12_DEPTH_STENCIL_DESC DepthStencilState_Disabled;
				D3D12_DEPTH_STENCIL_DESC DepthStencilState_ReadWrite_GE;
				D3D12_DEPTH_STENCIL_DESC DepthStencilState_ReadWrite_LT;
				D3D12_DEPTH_STENCIL_DESC DepthStencilState_ReadOnly_GE;
				D3D12_DEPTH_STENCIL_DESC DepthStencilState_ReadOnly_LT;
				D3D12_DEPTH_STENCIL_DESC DepthStencilState_ReadOnly_EQ;

				D3D12_RASTERIZER_DESC RasterizerDefault_CCW_CullBack;
				D3D12_RASTERIZER_DESC RasterizerDefault_CW_CullBack;
				D3D12_RASTERIZER_DESC RasterizerDefault_CCW_TwoSided;

				D3D12_BLEND_DESC BlendState_NoColorWrite;
				D3D12_BLEND_DESC BlendState_Disable;
				D3D12_BLEND_DESC BlendState_PreMultiplied;
				D3D12_BLEND_DESC BlendState_Traditional;
				D3D12_BLEND_DESC BlendState_Additive;
				D3D12_BLEND_DESC BlendState_TraditionalAdditive;
			}

			void InitializeCommonStateVars()
			{
#pragma region Depth Stencil State
				// @ ------------------	Depth Stencil State ------------------
				// @ Field Explanation:
				// @	- Depth Func: 
				// @		If *source data* <COMPARISON_FUNC> *destination data*, the comparison passes.
				// @	- Depth Write Mask:
				// @		When write to DSBuffer, dest_data = src_data & DSWriteMask;
				// @ ---------------------------------------------------------
				common::DepthStencilState_Disabled.DepthEnable = false;
				common::DepthStencilState_Disabled.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
				common::DepthStencilState_Disabled.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
				common::DepthStencilState_Disabled.StencilEnable = false;
				common::DepthStencilState_Disabled.StencilReadMask= D3D12_DEFAULT_STENCIL_READ_MASK;
				common::DepthStencilState_Disabled.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
				common::DepthStencilState_Disabled.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
				common::DepthStencilState_Disabled.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
				common::DepthStencilState_Disabled.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
				common::DepthStencilState_Disabled.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
				common::DepthStencilState_Disabled.BackFace = common::DepthStencilState_Disabled.FrontFace;

				common::DepthStencilState_ReadWrite_GE = common::DepthStencilState_Disabled;
				common::DepthStencilState_ReadWrite_GE.DepthEnable = TRUE;
				common::DepthStencilState_ReadWrite_GE.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
				common::DepthStencilState_ReadWrite_GE.DepthFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL;

				common::DepthStencilState_ReadWrite_LT = common::DepthStencilState_ReadWrite_GE;
				common::DepthStencilState_ReadWrite_LT.DepthFunc = D3D12_COMPARISON_FUNC_LESS;

				common::DepthStencilState_ReadOnly_GE = common::DepthStencilState_ReadWrite_GE;
				common::DepthStencilState_ReadOnly_GE.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

				common::DepthStencilState_ReadOnly_LT = common::DepthStencilState_ReadOnly_GE;
				common::DepthStencilState_ReadOnly_LT.DepthFunc = D3D12_COMPARISON_FUNC_LESS;

				common::DepthStencilState_ReadOnly_EQ = common::DepthStencilState_ReadOnly_GE;
				common::DepthStencilState_ReadOnly_EQ.DepthFunc = D3D12_COMPARISON_FUNC_EQUAL;
#pragma endregion

#pragma region Rasterizer States
				// @ ------------------	Rasterizer State ------------------
				// @ Field Explanation:
				// @	- Front Face: 
				// @		In DX, front face is defaultly clock-wise. Vertex Order Matters!
				// @	- Cull Mode:
				// @		User can select to cull front/back/none face
				// @ ---------------------------------------------------------
				common::RasterizerDefault_CCW_CullBack.FillMode = D3D12_FILL_MODE_SOLID;
				common::RasterizerDefault_CCW_CullBack.CullMode = D3D12_CULL_MODE_BACK;
				common::RasterizerDefault_CCW_CullBack.FrontCounterClockwise = TRUE;
				common::RasterizerDefault_CCW_CullBack.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
				common::RasterizerDefault_CCW_CullBack.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
				common::RasterizerDefault_CCW_CullBack.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
				common::RasterizerDefault_CCW_CullBack.DepthClipEnable = TRUE;
				common::RasterizerDefault_CCW_CullBack.MultisampleEnable = FALSE;
				common::RasterizerDefault_CCW_CullBack.AntialiasedLineEnable = FALSE;
				common::RasterizerDefault_CCW_CullBack.ForcedSampleCount = 0;
				common::RasterizerDefault_CCW_CullBack.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
				
				common::RasterizerDefault_CW_CullBack = common::RasterizerDefault_CCW_CullBack;
				common::RasterizerDefault_CW_CullBack.FrontCounterClockwise = FALSE;

				common::RasterizerDefault_CCW_TwoSided = common::RasterizerDefault_CCW_CullBack;
				common::RasterizerDefault_CCW_TwoSided.CullMode = D3D12_CULL_MODE_NONE;
#pragma endregion

#pragma region Rasterizer States
				// @ ------------------------------------	Blend State ------------------------------------
				// @ Field Explanation:
				// @	- IndependentBlendEnable: 
				// @		Set to TRUE to enable independent blending. 
				// @		If set to FALSE, only the RenderTarget[0] members are used;
				// @	- Cull Mode:
				// @		User can select to cull front/back/none face
				// @	- BlendOp / BlendFactor(Src&Dest)
				// @		(<SRC_BLEND> * psOutput) <BLEND_OP> (<DEST_BLEND> * destExisting) 
				// @	- Common BlendState:
				// @		- Pre-multiplied:		SrcBlend = 1,	DestBlend = 1 - As.
				// @								(mean the *src* has already been multiplied before d3d12 blend)
				// @		- Traditional:			SrcBlend = As,	DestBlend = 1 - As.
				// @		- Additive:				SrcBlend = 1,	DestBlend = 1.
				// @		- Traditional-Additive: SrcBlend = As,	DestBlend = 1.
				// @ ---------------------------------------------------------------------------------------
				D3D12_BLEND_DESC commonBD = {};
				commonBD.IndependentBlendEnable = FALSE;
				commonBD.RenderTarget[0].BlendEnable = FALSE;
				commonBD.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
				commonBD.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
				commonBD.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
				commonBD.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
				commonBD.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
				commonBD.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
				commonBD.RenderTarget[0].RenderTargetWriteMask = 0;
				common::BlendState_NoColorWrite = commonBD; 

				commonBD.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
				common::BlendState_Disable = commonBD;

				commonBD.RenderTarget[0].BlendEnable = TRUE;
				common::BlendState_Traditional = commonBD;

				commonBD.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
				common::BlendState_PreMultiplied = commonBD;

				commonBD.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
				common::BlendState_Additive = commonBD;

				commonBD.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
				common::BlendState_TraditionalAdditive = commonBD;
#pragma endregion
			}
		}
	}
}