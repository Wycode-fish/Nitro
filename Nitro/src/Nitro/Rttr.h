#pragma once

#define NT_TEMPLATE_CONDITION(cond) typename Nitro::EnableIf<cond, u32>::type* = nullptr
namespace Nitro
{
	/// a little RTTR here
	template <bool cond, typename T>
	struct EnableIf
	{};
	template <typename T>
	struct EnableIf<true, T>
	{
		typedef T type;
	};

	template <typename _BaseType, typename T>
	struct IsDerivedFrom
	{
	private:
		static constexpr bool _Check(_BaseType*) { return true; }
		static constexpr bool _Check(...) { return false; }
	public:
		enum { value = _Check(static_cast<T*>(nullptr)) };
	};

	// @ FASTER RTTI WITH MINIMUM OVERHEAD
	// @	- EACH CLASS USE THIS _MinimumTypeInfo STATIC VAR TO STORE INFO
	// @	- FASTER dynamic_cast AS C++ dynamic_cast INTRODUCE TOO MUCH STRING COMPARING.
	struct _MinimumTypeInfo
	{
		_MinimumTypeInfo* Parent;
		const char* Name;

		_MinimumTypeInfo(_MinimumTypeInfo* parent, const char* name)
			: Parent{ parent }, Name{ name }
		{}

		bool IsParentTo(_MinimumTypeInfo* info)
		{
			if (info == this)
				return true;
			while (info != nullptr)
			{
				if (this == info->Parent)
					return true;
				info = info->Parent;
			}
			return false;
		}
	};
#define RTTI_TYPENAME(...) __VA_ARGS__

#define DECLARE_MINIMAL_RTTI_BASE(TypeName) \
	static _MinimumTypeInfo sm_TypeInfo;\
	static _MinimumTypeInfo* GetStaticTypeInfo();\
	virtual _MinimumTypeInfo* GetDynamicTypeInfo() const;

#define DEFINE_MINIMAL_RTTI_BASE(TypeName)\
	_MinimumTypeInfo TypeName::sm_TypeInfo = {nullptr RTTI_TYPENAME(, #TypeName)};\
	_MinimumTypeInfo* TypeName::GetStaticTypeInfo() { return &TypeName::sm_TypeInfo; }\
	_MinimumTypeInfo* TypeName::GetDynamicTypeInfo() const { return GetStaticTypeInfo(); }
	
#define DECLARE_MINIMAL_RTTI(BaseTypeName, TypeName) \
	static _MinimumTypeInfo sm_TypeInfo;\
	static _MinimumTypeInfo* GetStaticTypeInfo();\
	virtual _MinimumTypeInfo* GetDynamicTypeInfo() const;

#define DEFINE_MINIMAL_RTTI(BaseTypeName, TypeName)\
	_MinimumTypeInfo TypeName::sm_TypeInfo = {&BaseTypeName::sm_TypeInfo RTTI_TYPENAME(, #TypeName)};\
	_MinimumTypeInfo* TypeName::GetStaticTypeInfo() { return &TypeName::sm_TypeInfo; }\
	_MinimumTypeInfo* TypeName::GetDynamicTypeInfo() const { return GetStaticTypeInfo(); }

	template <typename FROM, typename TO>
	TO mdynamic_cast(FROM& fromRef)
	{
		if (fromRef == NULL)
			return NULL;
		else
			return TO::GetStaticTypeInfo()->IsParentTo(fromRef->GetDynamicTypeInfo()) ? static_cast<TO>(fromRef) : NULL;
	}
}