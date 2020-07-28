#pragma once

#include "RefCount.h"

#include <iostream>

template<class T> class WeakPointer;
template<class T>
class SmartPointer
{
public:
	constexpr SmartPointer() noexcept {
		m_ObjectPointer = nullptr;
		m_Counter = nullptr;
	}
	constexpr SmartPointer(std::nullptr_t p_NullPtr) noexcept {
		m_ObjectPointer = nullptr;
		m_Counter = nullptr;
	}
	explicit SmartPointer(T* p_ObjectPointer) : m_ObjectPointer { p_ObjectPointer } {
		m_Counter = new RefCount(1, 0);
	}
	SmartPointer(const SmartPointer<T>& p_Other) {
		m_ObjectPointer = p_Other.m_ObjectPointer;
		m_Counter = p_Other.m_Counter;
		if (p_Other.m_Counter) {
			m_Counter->sp_count++;
		}
	}
	SmartPointer(WeakPointer<T>& p_WP) {
		if (p_WP.m_ObjectPointer) {
			m_ObjectPointer = p_WP.m_ObjectPointer;
			m_Counter = p_WP.m_Counter;
			m_Counter->sp_count++;
		}
		else {
			m_ObjectPointer = nullptr;
			m_Counter = nullptr;
		}
	}

	SmartPointer(SmartPointer&& p_Other) noexcept{
		m_ObjectPointer = p_Other.m_ObjectPointer;
		m_Counter = p_Other.m_Counter;
		p_Other.m_ObjectPointer = nullptr;
		p_Other.m_Counter = nullptr;

	}
	~SmartPointer() {
		Release();
	}
	
	SmartPointer<T>& operator=(SmartPointer<T> p_Other) {
		if (this != &p_Other) {
			Release();
			m_ObjectPointer = p_Other.m_ObjectPointer;
			m_Counter = p_Other.m_Counter;
			m_Counter->sp_count++;
		}
		return *this;
	}

	void operator=(std::nullptr_t p_NullPtr) {
		Release();
		m_ObjectPointer = nullptr;
		m_Counter = nullptr;
	}

	T* operator->() {
		if(m_ObjectPointer) return m_ObjectPointer;
		return nullptr;
	}
	T& operator*() {
		if (m_ObjectPointer) return (*m_ObjectPointer);
		else throw std::runtime_error("Try to access a nullptr.");
	}

	operator bool() const noexcept {
		return m_ObjectPointer != nullptr;
	}

	bool operator==(std::nullptr_t p_NullPtr) {
		return !m_ObjectPointer;
	}

	bool operator==(SmartPointer<T> const& p_Other) const{
		return m_ObjectPointer == p_Other.m_ObjectPointer;
	}


	bool operator!=(std::nullptr_t const& p_NullPtr) const{
		return m_ObjectPointer;
	}

	bool operator!=(SmartPointer<T> const& p_Other) const{
		return m_ObjectPointer != p_Other.m_ObjectPointer;
	}

	// For debug only
	void PrintCounter() {
		if (m_Counter) {
			std::cout << "Smart Pointer:" << m_Counter->sp_count << std::endl;
			std::cout << "Weak Pointer:" << m_Counter->wp_count << std::endl;

		}
		else {
			std::cout << "No pointers remained." << std::endl;
		}
	}

	// For cleaning up game engine
	void Clean() {
		if (m_ObjectPointer) {
			delete m_ObjectPointer;
		}
		if (m_Counter) {
			delete m_Counter;
		}
	}

	friend class WeakPointer<T>;
private:
	void Release() {
		if (m_Counter && (--(m_Counter->sp_count) == 0)) {
			if (m_ObjectPointer) {
				delete m_ObjectPointer;
			}

			m_ObjectPointer = nullptr;
			if (m_Counter->wp_count < 1) {
				delete m_Counter;
				m_Counter = nullptr;

			}
		}
	}

	T* GetObjectPtr() {
		return m_ObjectPointer;
	}

	RefCount* m_Counter;
	T* m_ObjectPointer;
};

template<class T>
inline bool operator==(std::nullptr_t i_nullptrconst, SmartPointer<T> const& p_Operand1) {
	return !p_Operand1;
}

template<class T>
inline bool operator!=(std::nullptr_t i_nullptr, SmartPointer<T> const& p_Operand1) {
	return p_Operand1;
}