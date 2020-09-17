#pragma once

#include "BaseOps.h"

namespace Nitro
{
	namespace Util
	{
		// @ ------------------------------------------------------------
		// @					Sort Ops
		// @ ------------------------------------------------------------
		template <typename _ElementType, u32 _ArraySize, NT_TEMPLATE_CONDITION((_ArraySize& (_ArraySize - 1)) == 0)>
		void bitonic_sort_cpu(_ElementType arr[], Util::ntDelegate<bool(_ElementType, _ElementType)> compareFunc)
		{
			for (u32 groupSize = 2; groupSize <= _ArraySize; groupSize <<= 1)
			{
				for (u32 gid = 0; gid < _ArraySize / groupSize; ++gid)
				{
					u32 opCode = BIT(0) & gid;
					u32 currentInnerGroupSize = groupSize;

					for (u32 currentInnerGroupSize = groupSize; currentInnerGroupSize > 1; currentInnerGroupSize >>= 1)
					{
						u32 currentInnerGroupCount = groupSize / currentInnerGroupSize;
						u32 compareDistance = currentInnerGroupSize >> 1;

						for (u32 currentInnerGroupIndex = 0; currentInnerGroupIndex < currentInnerGroupCount; ++currentInnerGroupIndex)
						{
							for (u32 eid = 0; eid < compareDistance; ++eid)
							{
								u32 left = gid * groupSize + currentInnerGroupIndex * currentInnerGroupSize + eid;
								_ElementType& l = arr[left];
								_ElementType& r = arr[left + compareDistance];

								bool comparePass = compareFunc(l, r);
								if ((comparePass && opCode == 0) || (!comparePass && opCode == 1))
								{
									std::swap(l, r);
								}
							}
						}
					}
				}
			}
		}

		template <typename _ElementType, u32 _ArraySize, NT_TEMPLATE_CONDITION((_ArraySize& (_ArraySize - 1)) == 0)>
		void bitonic_sort_cpu2(_ElementType arr[], Util::ntDelegate<bool(_ElementType, _ElementType)> compareFunc)
		{
			for (u32 groupSize = 2, groupCount = _ArraySize >> 1; groupSize <= _ArraySize; groupSize <<= 1, groupCount >>= 1)
			{
				for (u32 gid = 0; gid < groupCount; ++gid)
				{
					bitonic_sort_cpu2_helper(arr, groupSize, gid * groupSize, gid & BIT(0), compareFunc);
				}
			}
		}

		template <typename _ElementType>
		void bitonic_sort_cpu2_helper(_ElementType arr[], u32 groupSize, u32 elementIndex, u32 opCode, Util::ntDelegate<bool(_ElementType, _ElementType)> compareFunc)
		{
			if (groupSize == 1)
			{
				return;
			}
			u32 compareDistance = groupSize >> 1;
			for (u32 i = elementIndex; i < elementIndex + compareDistance; ++i)
			{
				bool comparePass = compareFunc(arr[i], arr[i + compareDistance]);
				if ((opCode == 0 && comparePass) || (opCode == 1 && !comparePass))
				{
					std::swap(arr[i], arr[i + compareDistance]);
				}
			}
			bitonic_sort_cpu2_helper(arr, compareDistance, elementIndex, opCode, compareFunc);
			bitonic_sort_cpu2_helper(arr, compareDistance, elementIndex + compareDistance, opCode, compareFunc);
		}
	}
}