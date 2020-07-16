#include "NtPCH.h"
#include "D3D12RootSignature.h"
#include "D3D12Context.h"
#include "Nitro/Application.h"

namespace Nitro
{
	namespace Graphics
	{
		namespace dx
		{
			std::unordered_map<hashkey_t, ID3D12RootSignature*> D3D12RootSignature::g_RootSignatureHashMap;

			void D3D12RootSignature::DestroyAll()
			{
				g_RootSignatureHashMap.clear();
			}

			D3D12RootSignature::D3D12RootSignature(unsigned int numOfParams, unsigned int numOfStaticSampler)
				: m_Params(nullptr)
				, m_StaticSamplers(nullptr)
				, m_IsReady(false)
			{
				this->Reset(numOfParams, numOfStaticSampler);
			}

			D3D12RootSignature::~D3D12RootSignature()
			{
				if (m_Params)
				{
					delete[] m_Params;
				}

				if (m_StaticSamplers)
				{
					delete[] m_StaticSamplers;
				}
			}

			D3D12RootParam& D3D12RootSignature::operator[](unsigned int idx)
			{
				NT_ASSERT(idx < m_NumOfParams, "root paramter retrieve idx out of bound.");
				return m_Params[idx];
			}

			const D3D12RootParam& D3D12RootSignature::operator[](unsigned int idx) const
			{
				NT_ASSERT(idx < m_NumOfParams, "root paramter retrieve idx out of bound.");
				return m_Params[idx];
			}

			void D3D12RootSignature::Reset(unsigned int numOfParams, unsigned int numOfStaticSampler)
			{
				if (m_Params)
				{
					delete[] m_Params;
				}

				if (numOfParams > 0) m_Params = new D3D12RootParam[numOfParams];
				else m_Params = nullptr;
				m_NumOfParams = numOfParams;

				if (m_StaticSamplers)
				{
					delete[] m_StaticSamplers;
				}
				
				if (numOfStaticSampler > 0) m_StaticSamplers = new D3D12_STATIC_SAMPLER_DESC[m_NumOfStaticSamplers];
				else m_StaticSamplers = nullptr;
				m_NumOfStaticSamplers = numOfStaticSampler;
				m_NumOfStaticSamplersInitialized = 0;
			}

			void D3D12RootSignature::Prepare(const std::string& name, D3D12_ROOT_SIGNATURE_FLAGS flags)
			{
				NT_ASSERT(m_NumOfStaticSamplers == m_NumOfStaticSamplersInitialized, "StaticSamplers haven't been all initialized.");

				if (m_IsReady) return;

				D3D12_ROOT_SIGNATURE_DESC desc = {};
				this->FillRootSignatureDesc(desc, flags);

				m_RootDescriptorTableBitMap_NonStaticSampler = 0;
				m_RootDescriptorTableBitMap_StaticSampler = 0;

				hashkey_t keyForThis = Nitro::Util::hash_state(&desc.Flags);
				keyForThis = Nitro::Util::hash_state(m_StaticSamplers, m_NumOfStaticSamplers, keyForThis);
				keyForThis = this->GenerateHashKey_PreProcessDescriptorTables(desc.pParameters, keyForThis);

				ID3D12RootSignature** signaturePtrAddr = nullptr;
				bool IsAlreadyInMap = this->TryRetrieveNativeRootSignatureFromStorage(keyForThis, signaturePtrAddr);

				if (!IsAlreadyInMap)	// then needs compile
				{
					m_NativeRootSignature = this->CreateNewRootSignature(D3D12Context::g_Device, desc);
#ifdef NT_DEBUG
					std::wstring _rsname = NT_STDSTR_TO_STDWSTR(name);
					m_NativeRootSignature->SetName(_rsname.c_str());
#endif // set name for debug
					g_RootSignatureHashMap[keyForThis] = m_NativeRootSignature;
					NT_ASSERT(*signaturePtrAddr == m_NativeRootSignature, "New root signature didn't register to predeemed address in global map");
				}
				else
				{
					while (*signaturePtrAddr == nullptr)
					{
						std::this_thread::yield();
					}
					m_NativeRootSignature = *signaturePtrAddr;
				}

				this->m_IsReady = true;
			}

			void D3D12RootSignature::InitNextStaticSampler(unsigned int registerID, const D3D12_SAMPLER_DESC& nonStaticSamplerDesc, D3D12_SHADER_VISIBILITY visibility)
			{
				NT_ASSERT(m_NumOfStaticSamplers > m_NumOfStaticSamplersInitialized, "Signature needs to be realized.");
				
				D3D12_STATIC_SAMPLER_DESC& staticSamplerDesc = m_StaticSamplers[m_NumOfStaticSamplersInitialized];
				staticSamplerDesc.AddressU = nonStaticSamplerDesc.AddressU;
				staticSamplerDesc.AddressV = nonStaticSamplerDesc.AddressV;
				staticSamplerDesc.AddressW = nonStaticSamplerDesc.AddressW;
				staticSamplerDesc.Filter = nonStaticSamplerDesc.Filter;
				staticSamplerDesc.MaxAnisotropy = nonStaticSamplerDesc.MaxAnisotropy;
				staticSamplerDesc.ComparisonFunc = nonStaticSamplerDesc.ComparisonFunc;
				staticSamplerDesc.MaxLOD = nonStaticSamplerDesc.MaxLOD;
				staticSamplerDesc.MinLOD = nonStaticSamplerDesc.MinLOD;
				// ---
				staticSamplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
				staticSamplerDesc.RegisterSpace = 0;
				staticSamplerDesc.ShaderRegister = registerID;
				staticSamplerDesc.ShaderVisibility = visibility;

				// deal with address mode border, to determine which static color to assign to 'BorderColor'
				if (staticSamplerDesc.AddressU == D3D12_TEXTURE_ADDRESS_MODE_BORDER ||
					staticSamplerDesc.AddressV == D3D12_TEXTURE_ADDRESS_MODE_BORDER ||
					staticSamplerDesc.AddressW == D3D12_TEXTURE_ADDRESS_MODE_BORDER)
				{
					if (nonStaticSamplerDesc.BorderColor[3] == 1.0f)
					{
						if (nonStaticSamplerDesc.BorderColor[0] == 0.0f)
						{
							staticSamplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
						}
						else
						{
							staticSamplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
						}
					}
					else
					{
						staticSamplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
					}
				}
				m_NumOfStaticSamplers++;
			}

			hashkey_t D3D12RootSignature::GenerateHashKey_PreProcessDescriptorTables(const D3D12_ROOT_PARAMETER* rootParams,  hashkey_t originHKVal)
			{
				hashkey_t keyForThis = originHKVal;

				for (u32 i = 0; i < m_NumOfParams; ++i)
				{
					m_ArrayRootDescriptorTableSizesInNum[i] = 0;

					const D3D12_ROOT_PARAMETER& currentNativeParam = rootParams[i];
					if (this->IsCurrentNativeParamOfTypeDescriptorTable(currentNativeParam.ParameterType))
					{
						NT_ASSERT(
							currentNativeParam.DescriptorTable.pDescriptorRanges != nullptr,
							"current root param of DescriptorTable type hasn't get Init_AsDescirptorTable yet."
						);

						const D3D12_DESCRIPTOR_RANGE* firstRange = currentNativeParam.DescriptorTable.pDescriptorRanges + i;
						size_t							rangeNum = currentNativeParam.DescriptorTable.NumDescriptorRanges;

						keyForThis = Nitro::Util::hash_state(firstRange, rangeNum, keyForThis);

						if (firstRange->RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER)
						{
							m_RootDescriptorTableBitMap_StaticSampler |= BIT(i);
						}
						else
						{
							m_RootDescriptorTableBitMap_NonStaticSampler |= BIT(i);
						}
						for (u32 rangeIdx = 0; rangeIdx < rangeNum; ++rangeIdx)
						{
							m_ArrayRootDescriptorTableSizesInNum[i] += currentNativeParam.DescriptorTable.pDescriptorRanges[rangeIdx].NumDescriptors;
						}
					}
					else
					{
						keyForThis = Nitro::Util::hash_state(&currentNativeParam, 1, keyForThis);
					}
				}
				return keyForThis;
			}

			bool D3D12RootSignature::TryRetrieveNativeRootSignatureFromStorage(hashkey_t keyForThis, /*ref_param*/ID3D12RootSignature**& rsRef)
			{
				static std::mutex internal_hashmap_mutex;
				std::lock_guard<std::mutex> internal_lock(internal_hashmap_mutex);

				bool alreadyInMap = false;
				auto iter = g_RootSignatureHashMap.find(keyForThis);
				if (iter == g_RootSignatureHashMap.end())
				{
					// @ automatically create new <hashkey, id3drs*> pair-entry by calling g_rshashmap[newkey]
					rsRef = &g_RootSignatureHashMap[keyForThis];
					alreadyInMap = false;
				}
				else
				{
					rsRef = &(iter->second);
					alreadyInMap = true;
				}

				return alreadyInMap;
			}

			ID3D12RootSignature* D3D12RootSignature::CreateNewRootSignature(ID3D12Device* device, const D3D12_ROOT_SIGNATURE_DESC& desc)
			{
				ID3D12RootSignature* res;
				ID3DBlob* pOutBlob, * pErrorBlob;
				NT_ASSERT(
					SUCCEEDED(D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &pOutBlob, &pErrorBlob)),
					"Compilation of root signature failed."
				);

				NT_ASSERT(
					SUCCEEDED(device->CreateRootSignature(0, pOutBlob->GetBufferPointer(), pOutBlob->GetBufferSize(), IID_PPV_ARGS(&res))),
					"Creation of root signature failed."
				);

				return res;
			}
		}
	}
}
