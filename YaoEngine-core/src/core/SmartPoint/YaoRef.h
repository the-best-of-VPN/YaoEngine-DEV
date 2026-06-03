#pragma once
#include<atomic>
namespace YaoEngine {
	class YaoRefcount
	{
	public:
		virtual ~YaoRefcount() = default;

		inline void AddRef()
		{
			m_refCount.fetch_add(1, std::memory_order_relaxed);
		}

		inline void DecRef()
		{
			if (m_refCount.fetch_sub(1, std::memory_order_acq_rel) == 1)
			{
				delete this;
			}
		}

		uint32_t RefCount() const
		{
			return m_refCount.load();
		}

	private:
		std::atomic<uint32_t> m_refCount = 0;
	};
	template<class T>
	class YaoRef{
	public:
		template<class ...Arg>
		static YaoRef<T> Create(Arg&&... args) {
			static_assert(std::is_base_of<YaoRefcount, T>::value, "T must be derived from YaoRefcount");
			return YaoRef<T>(new T(std::forward<Arg>(args)...));
		}
		YaoRef() = default;
		YaoRef(T* ref) : m_ref(ref) {
			static_assert(std::is_base_of<YaoRefcount, T>::value, "T must be derived from YaoRefcount");
			ref->AddRef();
		}
		YaoRef(const YaoRef& other) noexcept : m_ref(other.m_ref) {
			static_assert(std::is_base_of<YaoRefcount, T>::value, "T must be derived from YaoRefcount");
			if (m_ref) {
				m_ref->AddRef();
			}
		}
		YaoRef(YaoRef&& other) noexcept : m_ref(other.m_ref) {
			other.m_ref = nullptr;
		}
		YaoRef(YaoRef& other) noexcept : m_ref(other.m_ref) {
			static_assert(std::is_base_of<YaoRefcount, T>::value, "T must be derived from YaoRefcount");
			if (m_ref) {
				m_ref->AddRef();
			}
		}
		YaoRef(nullptr_t) {
			m_ref = nullptr;
		}
		~YaoRef() {
			if (m_ref) {
				m_ref->DecRef();
			}
		}
		Ref operator=(const YaoRef& other) noexcept {
			if (this != &other) {
				if (m_ref) {
					m_ref->DecRef();
				}
				m_ref = other.m_ref;
				if (m_ref) {
					m_ref->AddRef();
				}
			}
			return *this;
		}
		Ref operator=(YaoRef&& other) noexcept {
			if (this != &other) {
				if (m_ref) {
					m_ref->DecRef();
				}
				m_ref = other.m_ref;
				other.m_ref = nullptr;
			}
			return *this;
		}
		T* Row()
		{
			return m_ref;
		}
		const T* Row() const
		{
			return m_ref;
		}
		T& operator*()
		{
			return *m_ref;
		}

		const T& operator*() const
		{
			return *m_ref;
		}
		T* operator->() {
			return m_ref;
		}
		const T* operator->() const {
			return m_ref;
		}
		operator bool() const
		{
			return m_ref != nullptr;
		}
	private:
		T* m_ref = nullptr;
	};
}