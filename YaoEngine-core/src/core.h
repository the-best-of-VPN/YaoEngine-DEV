#pragma once
#include<unordered_map>
#include<memory>

#ifdef DEBUG
#define YAO_DEBUG_ONLY(x) do { x; } while(0)
#else
#define YAO_DEBUG_ONLY(x) do {} while(0)
#endif
#define BIT(x) (1u << x)
namespace YaoEngine {
#ifndef MYSTD_SMART_PTR
	template<class  T>
	using Ref = std::shared_ptr<T>;
	template<class  T>
	Ref<T>CreateRef() {
		return std::make_shared<T>();
	}
	template<class  T,class  ...Arg>
	Ref <T>CreateRef(Arg&& ...arg) {
		return std::make_shared<T>(std::forward<Arg>(arg)...);
	}
	template<class  T>
	using Scope = std::unique_ptr<T>;
	template<class  T>
	Scope<T>CreateScope() {
		return std::make_unique<T>();
	}
	template<class  T, class  ...Arg>
	Scope <T>CreateScope(Arg&& ...arg) {
		return std::make_unique<T>(std::forward<Arg>(arg)...);
	}

	template<class  T1,class T2>
	using HashMap = std::unordered_map<T1, T2>;
#else
//////
#endif
}