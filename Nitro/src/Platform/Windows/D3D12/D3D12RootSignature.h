#pragma once
#include <d3d12.h>

namespace Nitro
{
	namespace Graphics
	{
		namespace dx
		{
			// @ -------------------------------------------------------------------
			// @							RootParameter
			// @ -------------------------------------------------------------------
			struct D3D12RootParam
			{
#pragma region Variables
				D3D12_ROOT_PARAMETER NativeParam;
#pragma endregion

#pragma region Constructor/Destructor
				inline D3D12RootParam() { NativeParam.ParameterType = (D3D12_ROOT_PARAMETER_TYPE)0xffffffff; }
				inline ~D3D12RootParam() { this->Clear(); }
#pragma endregion

#pragma region Control Methods
				inline void Clear()
				{
					if (NativeParam.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
					{
						delete[] NativeParam.DescriptorTable.pDescriptorRanges;
					}
					NativeParam.ParameterType = (D3D12_ROOT_PARAMETER_TYPE)0xffffffff;
				}
				// @ RootParam of type 32bit-constant takes signature space of 1 DWORD for each 32bit const. 
				inline void Init_AsConstants(unsigned int shaderRegistry, unsigned int numOfConstants, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
				{
					NativeParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
					NativeParam.Constants.Num32BitValues = numOfConstants;
					NativeParam.Constants.RegisterSpace = 0;
					NativeParam.Constants.ShaderRegister = shaderRegistry;
					NativeParam.ShaderVisibility = visibility;
				}
				// @ RootParam of type descriptor takes signature space of 2 DWORDs. 
				inline void Init_AsDescriptor(unsigned int shaderRegistry, D3D12_ROOT_PARAMETER_TYPE descriptorType, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
				{
					NativeParam.ParameterType = descriptorType;
					NativeParam.Descriptor.RegisterSpace = 0;
					NativeParam.Descriptor.ShaderRegister = shaderRegistry;
					NativeParam.ShaderVisibility = visibility;
				}
				// @ RootParam of type descriptor table takes signature space of 1 DWORD. 
				inline void Init_AsDescriptorTable(unsigned int numOfRanges, D3D12_DESCRIPTOR_RANGE_TYPE initRangeType, unsigned int initRangeSizeInNum, unsigned int initBaseShaderRegistry, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
				{
					NativeParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
					NativeParam.DescriptorTable.NumDescriptorRanges = numOfRanges;
					NativeParam.DescriptorTable.pDescriptorRanges = new D3D12_DESCRIPTOR_RANGE[numOfRanges];
					NativeParam.ShaderVisibility = visibility;

					// @ Initialize at least 1 range inside the RootDescirptorTable.
					this->SetRangeAtIndexInDescriptorTable(0, initRangeType, initRangeSizeInNum, initBaseShaderRegistry, 0);
				}
				inline void Init_AsDescriptorRange(
					D3D12_DESCRIPTOR_RANGE_TYPE rangeType, 
					unsigned int baseShaderRegistry, 
					unsigned int descriptorCount,
					D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
				{
					Init_AsDescriptorTable(1, rangeType, descriptorCount, baseShaderRegistry, visibility);
				}
				// @ Every range in a talbe can have its own DescriptorRangeType.
				inline void SetRangeAtIndexInDescriptorTable(
					unsigned int				rangeIdx,
					D3D12_DESCRIPTOR_RANGE_TYPE rangeType,
					unsigned int				numOfDescriptorInRange,
					unsigned int				baseShaderRegistry,
					unsigned int				registerSpace)
				{
					D3D12_DESCRIPTOR_RANGE* range = const_cast<D3D12_DESCRIPTOR_RANGE*>(&NativeParam.DescriptorTable.pDescriptorRanges[rangeIdx]);
					range->BaseShaderRegister = baseShaderRegistry;
					range->NumDescriptors = numOfDescriptorInRange;
					range->RangeType = rangeType;
					range->RegisterSpace = registerSpace;
					range->OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
				}
#pragma endregion

			};

			// @ -------------------------------------------------------------------
			// @							RootSignature
			// @ -------------------------------------------------------------------
			// @ Maximum 64 DWORDS divied up amongst all root parameters.
			// @		- Root constants						= 1 DWORD * NumConstants
			// @		- Root descriptor (CBV, SRV, or UAV)	= 2 DWORDs each
			// @		- Descriptor table pointer				= 1 DWORD
			// @		- Static samplers						= 0 DWORDS (compiled into shader)
			// @ -------------------------------------------------------------------
			class D3D12RootSignature
			{
			public:
				static std::unordered_map<hashkey_t, ID3D12RootSignature*> g_RootSignatureHashMap;
				static void DestroyAll();

#pragma region Constructor/Destructor/operator
			public:
				D3D12RootSignature(unsigned int numOfParams = 0, unsigned int numOfStaticSampler = 0);
				virtual ~D3D12RootSignature();
				D3D12RootParam& operator[](unsigned int idx);
				const D3D12RootParam& operator[](unsigned int idx) const;
#pragma endregion

#pragma region Control Methods
			public:
				void	Reset(unsigned int numOfParams, unsigned int numOfStaticSampler);
				void	Prepare(const std::string& name, D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_NONE);
				void	InitNextStaticSampler(unsigned int registerID, const D3D12_SAMPLER_DESC& samplerDesc, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
#pragma endregion

			public:
				inline ID3D12RootSignature*			GetNativeRootSignature() const { return m_NativeRootSignature; }
				inline u32							GetNumOfParams() const { return m_NumOfParams; }
				inline u32							GetRootDescriptorTableBitMap_Sampler() const { return m_RootDescriptorTableBitMap_StaticSampler; }
				inline u32							GetRootDescriptorTableBitMap_NonSampler() const { return m_RootDescriptorTableBitMap_NonStaticSampler; }
				inline u32							GetRootDescriptorTableSizeAtIdx(u32 idx) const { return m_ArrayRootDescriptorTableSizesInNum[idx]; }
				inline D3D12RootParam&				GetParamRef(u32 idx) { return m_Params[idx]; }
			private:
				inline bool	IsCurrentNativeParamOfTypeDescriptorTable(D3D12_ROOT_PARAMETER_TYPE paramType) const
				{
					return paramType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
				}
				inline void FillRootSignatureDesc(D3D12_ROOT_SIGNATURE_DESC& desc, D3D12_ROOT_SIGNATURE_FLAGS flags) const
				{
					desc.Flags = flags;
					desc.NumParameters = m_NumOfParams;
					desc.NumStaticSamplers = m_NumOfStaticSamplers;
					desc.pParameters = reinterpret_cast<const D3D12_ROOT_PARAMETER*>(m_Params);
					desc.pStaticSamplers = reinterpret_cast<const D3D12_STATIC_SAMPLER_DESC*>(m_StaticSamplers);
				}
				// @ Generate hashkey for *this* rootsignature so we can store pair in map
				// @			* meanwhile, separate Sampler/Non-Sampler DescriptorTableType root param.
				hashkey_t					GenerateHashKey_PreProcessDescriptorTables(const D3D12_ROOT_PARAMETER* rootParams, hashkey_t originHKVal);
				
				// @ Retrieve native root signature stored in global hashmap
				bool						TryRetrieveNativeRootSignatureFromStorage(hashkey_t keyForThis, /*ref_param*/ID3D12RootSignature**& rsRef);
			
				// @ Create a new d3d12rootsignature
				ID3D12RootSignature*		CreateNewRootSignature(ID3D12Device* device, const D3D12_ROOT_SIGNATURE_DESC& desc);

#pragma region Variables
			protected:
				D3D12RootParam*				m_Params;
				unsigned int				m_NumOfParams;
				unsigned int				m_RootDescriptorTableBitMap_NonStaticSampler;
				unsigned int				m_RootDescriptorTableBitMap_StaticSampler;
				unsigned int				m_ArrayRootDescriptorTableSizesInNum[16];

				D3D12_STATIC_SAMPLER_DESC*	m_StaticSamplers;
				unsigned int				m_NumOfStaticSamplers;
				unsigned int				m_NumOfStaticSamplersInitialized;

				ID3D12RootSignature*		m_NativeRootSignature;
				bool						m_IsReady;
#pragma endregion
			};
		}
	}
}