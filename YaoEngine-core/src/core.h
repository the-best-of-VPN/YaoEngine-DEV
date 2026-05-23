#include<unordered_map>
#include<memory>

namespace YaoEngine {
#ifdef STD_SMART_PTR
	template<class  T>
	using Ref = std::shared_ptr<T>;
	template<class  T>
	Ref<T>Create() {
		return std::make_shared<T>();
	}
	template<class  T,class  ...Arg>
	Ref <T>Create(Arg&& ...arg) {
		return std::make_shared<T>(std::forward<Arg>(arg)...);
	}
	template<class  T1,class T2>
	using Hash = std::unordered_map<T1, T2>;
#elif
//////
#endif
}