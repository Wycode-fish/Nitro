// @ Note:
// @	- This is used as a helper for delegate.h
// @	- No "#pragma once" declared ahead, as content of this file will get included in one file multiple times.

#if DLGT_PARAM_NUM > 0
#define DLGT_COMMA ,
#else
#define DLGT_COMMA
#endif

#define NT_ENUM_0(x)
#define NT_ENUM_1(x) x##0
#define NT_ENUM_2(x) NT_ENUM_1(x), x##1
#define NT_ENUM_3(x) NT_ENUM_2(x), x##2
#define NT_ENUM_4(x) NT_ENUM_3(x), x##3
#define NT_ENUM_5(x) NT_ENUM_4(x), x##4
#define NT_ENUM_I(N, x) NT_ENUM_##N(x)
#define NT_ENUM(N, x) NT_ENUM_I(N, x)

#define NT_PARAM_ENUM_0(PType, PName)
#define NT_PARAM_ENUM_1(PType, PName) PType##0   PName##0
#define NT_PARAM_ENUM_2(PType, PName) NT_PARAM_ENUM_1(PType, PName), PType##1   PName##1
#define NT_PARAM_ENUM_3(PType, PName) NT_PARAM_ENUM_2(PType, PName), PType##2   PName##2
#define NT_PARAM_ENUM_4(PType, PName) NT_PARAM_ENUM_3(PType, PName), PType##3   PName##3
#define NT_PARAM_ENUM_5(PType, PName) NT_PARAM_ENUM_4(PType, PName), PType##4   PName##4
#define NT_PARAM_ENUM_I(N, PType, PName) NT_PARAM_ENUM_##N(PType, PName)		 
#define NT_PARAM_ENUM(N, PType, PName) NT_PARAM_ENUM_I(N, PType, PName)

// @ e.g. NT_ARGS(3, a) --> a0, a1, a2
#define NT_ARGS(N, x) NT_ENUM(N, x)
#define DLGT_ARGS(x) NT_ARGS(DLGT_PARAM_NUM, x)

// @ e.g. NT_PARAMS(3, A, a) --> A0 a0, A1 a1, A2 a2
#define NT_PARAMS(N, P, p) NT_PARAM_ENUM(N, P, p)
#define DLGT_PARAMS(P, p) NT_PARAMS(DLGT_PARAM_NUM, P, p)

#define NT_PASTE_I(a, b)	a##b
#define NT_PASTE(a, b)		NT_PASTE_I(a, b) 
#define DLGT_NAME			NT_PASTE( base_delegate, DLGT_PARAM_NUM )

namespace Nitro
{
	namespace Util
	{
		template <typename R DLGT_COMMA DLGT_ARGS(typename P) >
		class DLGT_NAME
		{
			// @ ??? this one seems useless
			template <typename TypeObj>
			struct objType_from_MF;

			template <typename TypeObj, typename TypeRet DLGT_COMMA DLGT_ARGS(typename TypeArg)>
			struct objType_from_MF< TypeRet(TypeObj::*)(DLGT_ARGS(TypeArg)) >
			{
				typedef TypeObj type;
			};

			template <typename TypeObj, typename TypeRet DLGT_COMMA DLGT_ARGS(typename TypeArg)>
			struct objType_from_MF< TypeRet(TypeObj::*)(DLGT_ARGS(TypeArg)) const >
			{
				typedef TypeObj type;
			};

			template <typename TypeObj>
			struct match_signature
			{
				struct yes { char x[1]; };
				struct no { char x[8]; };

				static no is_match(...);
				static yes is_match(R(TypeObj::*)(DLGT_ARGS(P)));
				static yes is_match(R(TypeObj::*)(DLGT_ARGS(P)) const);
			};

		public:
			DLGT_NAME()
			{
				this->reset();
			}

			template <typename TypeObj, typename TypeMF>
			DLGT_NAME(TypeObj obj, TypeMF mfunc)
			{
				this->bind(obj, mfunc);
			}

			DLGT_NAME(R(*freeFunc)(DLGT_ARGS(P)))
			{
				this->bind(freeFunc);
			}

			void reset()
			{
				m_target = nullptr;
				m_func.m_memberFunc = nullptr;
			}

			template <typename TypeObj, typename TypeMF>
			void bind(TypeObj* obj, TypeMF func)
			{
				// @ test if the signature of the input-param func (after getting rid of obj type) match the delegate class
				COMPILETIME_ASSERT(sizeof(match_signature<typename objType_from_MF<TypeMF>::type>::is_match(func)) == sizeof(typename match_signature<typename objType_from_MF<TypeMF>::type>::yes));

				this->reset();
				m_target = fwDelegate::BindAssitant(obj, (objType_from_MF<TypeMF>::type*)0, func, m_func.m_memberFunc);
			}

			void bind(R(*freeFuncPtr)(DLGT_ARGS(P)))
			{
				this->reset();
				m_func.m_freeFunc = freeFuncPtr;
			}

			R invoke(DLGT_PARAMS(P, p)) const
			{
				return m_target ? (m_target->*m_func.m_memberFunc)(DLGT_ARGS(p)) : (*m_func.m_freeFunc)(DLGT_ARGS(p));
			}

			R operator()(DLGT_PARAMS(P, p)) const
			{
				return this->invoke(DLGT_ARGS(p));
			}

			bool operator==(const DLGT_NAME<R DLGT_COMMA DLGT_ARGS(P)>& other)
			{
				return m_target == other.m_target && this->m_func
			}
		protected:
			fwDelegate::CGenericClass* m_target;

			union {
				R(fwDelegate::CGenericClass::* m_memberFunc)(DLGT_ARGS(P));
				R(*m_freeFunc)(DLGT_ARGS(P));
			} m_func;
		};

		template <typename R DLGT_COMMA DLGT_ARGS(typename P)>
		class ntDelegate< R(DLGT_ARGS(P)) > : public DLGT_NAME<R DLGT_COMMA DLGT_ARGS(P)>
		{
		public:
			template <typename TypeObj, typename TypeFunc>
			ntDelegate(TypeObj* obj, TypeFunc func)
			{
				this->bind(obj, func);
			}

			ntDelegate() {}

			explicit ntDelegate(R(*freeFunc)(DLGT_ARGS(P)))
			{
				this->bind(freeFunc);
			}
		};
	}
}