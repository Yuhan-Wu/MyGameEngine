#pragma once
#include "RefCount.h"
#include "SmartPointer.h"

template<class T>
class WeakPointer
{
public:
	constexpr WeakPointer() noexcept {
		object_pointer = nullptr;
		counter = nullptr;
	}
	constexpr WeakPointer(std::nullptr_t i_nullptr) noexcept {
		object_pointer = nullptr;
		counter = nullptr;
	}
	WeakPointer(SmartPointer<T>& sp) : object_pointer{ sp.object_pointer }, counter{ sp.counter } {
		counter->wp_count++;
	}

	WeakPointer(const WeakPointer<T>& other) {
		object_pointer = other.object_pointer;
		counter = other.counter;
	}
	WeakPointer(WeakPointer<T>&& other) noexcept {
		object_pointer = other.object_pointer;
		counter = other.counter;
		other.object_pointer = nullptr;
		other.counter = nullptr;
	}
	~WeakPointer() {
		Release();
	}

	WeakPointer<T>& operator=(const WeakPointer<T>& other) {
		if (this != &other) {
			Release();
			object_pointer = other.object_pointer;
			counter = other.counter;
			counter->wp_count++;
		}
		return *this;
	}

	WeakPointer<T>& operator=(const SmartPointer<T>& other) {
		Release();
		object_pointer = other.object_pointer;
		counter = other.counter;
		counter->wp_count++;
		return *this;
	}

	void operator=(std::nullptr_t i_nullptr) {
		Release();
		object_pointer = nullptr;
		counter = nullptr;
	}

	SmartPointer<T> Get() {
		if (counter->sp_count > 0) {
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
		if (counter) {
			--counter->wp_count;
			if (counter->sp_count < 1 && counter->wp_count < 1) {
				delete counter;
				counter = nullptr;
			}
		}
	}

	RefCount* counter;
	T* object_pointer;
};

template<class T>
inline bool operator==(WeakPointer<T>& operand1, WeakPointer<T>& operand2) {
	if (operand1 && operand2) return *operand1 == *operand2;
	return false;
}

template<class T>
inline bool operator==(const WeakPointer<T>& operand1, std::nullptr_t i_nullptr) {
	return !operand1;
}

template<class T>
inline bool operator==(std::nullptr_t i_nullptrconst, WeakPointer<T>& operand1) {
	return !operand1;
}

template<class T>
inline bool operator!=(WeakPointer<T>& operand1, WeakPointer<T>& operand2) {
	if (operand1 && operand2) return *operand1 != *operand2;
	return false;
}

template<class T>
inline bool operator!=(const WeakPointer<T>& operand1, std::nullptr_t i_nullptrconst) {
	return operand1;
}

template<class T>
inline bool operator!=(std::nullptr_t i_nullptr, const WeakPointer<T>& operand1) {
	return operand1;
}