#pragma once

#include "UtilConfig.h"

namespace Nitro
{
	namespace Util
	{
		namespace fwDelegate
		{
			// @ no member var
			struct CGenericClass
			{
				CGenericClass* getThz() { return this; }
			};

			// @ remove const or volatile qualification
			template <typename T>
			struct removeCV
			{
				typedef T type;
			};
			template <typename T>
			struct removeCV<T const>
			{
				typedef T type;
			};
			template <typename T>
			struct removeCV<volatile T>
			{
				typedef T type;
			};
			template <typename T>
			struct removeCV<const volatile T>
			{
				typedef T type;
			};

			template <typename TypeObj_Input, typename TypeObj_Deduct, typename TypeMF, typename TypeGenericBoundMF>
			CGenericClass* BindAssitant(TypeObj_Input* obj, TypeObj_Deduct*, TypeMF memberFunc, TypeGenericBoundMF& boundMF)
			{
				// @ the pragma macro here is to make sure there's no padding in memory layout of this union
				// @ by setting the alignment val of compiler to 1, and later on set it back to original alignment value.
#pragma pack(push, 1)
				union {
					TypeMF _memberFunc;
					TypeGenericBoundMF _boundMF;
					CGenericClass* (TypeObj_Deduct::* _probeSelf)();
					CGenericClass* (CGenericClass::* _getSelf)();
				} memblk;
#pragma pack(pop)
				COMPILETIME_ASSERT(sizeof(memberFunc) == sizeof(memblk));

				// @ member functions can be casted between class, as long as the signature match, 
				// @ and don't make invalid memory access to object of new type in code.
				// @ since member function ptrs are just ptrs to text code area in memory, and the
				// @ things special about them are:
				// @	1. they have "this" as a implicit parameter in param list.
				// @	2. the ptr size of them are varied, could be 4, 8, 12, 16 bytes, as they need to store offset value in obj memory layout.
				memblk._memberFunc = memberFunc;
				boundMF = memblk._boundMF;

				memblk._getSelf = &CGenericClass::getThz;

				// @ note that here we're throwing a TypeObj_Input object to a member function of TypeObj_Deduct,
				// @ input obj is implicitly casted: TypeObj_Input -> TypeObj_Deduct, 
				// @ this cast is done by compiler, the compiler will decide 
				// @ this needs to be done this way ()
				return (const_cast<typename removeCV<TypeObj_Input>::type*>(obj)->*memblk._probeSelf)();
			}
		}

		template <typename T> class ntDelegate;
	}
}
#undef DLGT_PARAM_NUM

#define DLGT_PARAM_NUM 0
#include "DelegateTemplate.h"
#undef DLGT_PARAM_NUM

#define DLGT_PARAM_NUM 1
#include "DelegateTemplate.h"
#undef DLGT_PARAM_NUM

#define DLGT_PARAM_NUM 2
#include "DelegateTemplate.h"
#undef DLGT_PARAM_NUM

#define DLGT_PARAM_NUM 3
#include "DelegateTemplate.h"
#undef DLGT_PARAM_NUM

#define DLGT_PARAM_NUM 4
#include "DelegateTemplate.h"
#undef DLGT_PARAM_NUM

#define DLGT_PARAM_NUM 5
#include "DelegateTemplate.h"
#undef DLGT_PARAM_NUM