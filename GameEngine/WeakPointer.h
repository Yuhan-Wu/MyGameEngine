#pragma once
#include "RefCount.h"
#include "SmartPointer.h"

template<class T>
class WeakPointer
{
public:
	constexpr WeakPointer() noexcept {
		m_ObjectPointer = nullptr;
		m_Counter = nullptr;
	}
	constexpr WeakPointer(std::nullptr_t p_NullPtr) noexcept {
		m_ObjectPointer = nullptr;
		m_Counter = nullptr;
	}
	WeakPointer(SmartPointer<T>& p_SP) : m_ObjectPointer{ p_SP.object_pointer }, m_Counter{ p_SP.counter } {
		m_Counter->wp_count++;
	}
	WeakPointer(const WeakPointer<T>& p_Other) {
		m_ObjectPointer = p_Other.m_ObjectPointer;
		m_Counter = p_Other.m_Counter;
	}
	WeakPointer(WeakPointer<T>&& p_Other) noexcept {
		m_ObjectPointer = p_Other.m_ObjectPointer;
		m_Counter = p_Other.m_Counter;
		p_Other.m_ObjectPointer = nullptr;
		p_Other.m_Counter = nullptr;
	}
	~WeakPointer() {
		Release();
	}


	WeakPointer<T>& operator=(WeakPointer<T> p_Other) {
		if (this != &p_Other) {
			Release();
			m_ObjectPointer = p_Other.m_ObjectPointer;
			m_Counter = p_Other.m_Counter;
			m_Counter->wp_count++;
		}
		return *this;
	}


	WeakPointer<T>& operator=(SmartPointer<T> p_Other) {

		Release();
		m_ObjectPointer = p_Other.m_ObjectPointer;
		m_Counter = p_Other.m_Counter;
		m_Counter->wp_count++;
		return *this;
	}

	void operator=(std::nullptr_t p_NullPtr) {
		Release();
		m_ObjectPointer = nullptr;
		m_Counter = nullptr;
	}

	operator bool() const noexcept {
		return m_ObjectPointer != nullptr;
	}

	bool operator==(std::nullptr_t p_NullPtr) {
		return !m_ObjectPointer;
	}

	bool operator==(WeakPointer<T> const& p_Other) const {
		return m_ObjectPointer == p_Other.m_ObjectPointer;
	}


	bool operator!=(std::nullptr_t const& p_NullPtr) const {
		return m_ObjectPointer;
	}

	bool operator!=(WeakPointer<T> const& p_Other) const {
		return m_ObjectPointer != p_Other.m_ObjectPointer;
	}

	SmartPointer<T> Get() {
		if (m_Counter->sp_count > 0) {
			return SmartPointer<T>(*this);
		}
		else {
			Release();
			return SmartPointer<T>();
		}
		
	}



	friend class SmartPointer<T>;
private:
	void Release() {
		if (m_Counter) {
			--m_Counter->wp_count;
			if (m_Counter->sp_count < 1 && m_Counter->wp_count < 1) {
				delete m_Counter;
				m_Counter = nullptr;

			}
		}
	}

	RefCount* m_Counter;
	T* m_ObjectPointer;
};

template<class T>
inline bool operator==(std::nullptr_t i_nullptrconst, WeakPointer<T> const& p_Operand1) {
	return !p_Operand1;
}

template<class T>
inline bool operator!=(std::nullptr_t i_nullptr, WeakPointer<T> const& p_Operand1) {
	return p_Operand1;
}

