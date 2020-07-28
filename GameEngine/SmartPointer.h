#pragma once
#include "RefCount.h"
#include <iostream>

template<class T> class WeakPointer;
template<class T>
class SmartPointer
{
public:
	constexpr SmartPointer() noexcept {
		object_pointer = nullptr;
		counter = nullptr;
	}
	constexpr SmartPointer(std::nullptr_t i_nullptr) noexcept {
		object_pointer = nullptr;
		counter = nullptr;
	}
	explicit SmartPointer(T* object_pointer) : object_pointer { object_pointer } {
		counter = new RefCount(1, 0);
	}
	SmartPointer(const SmartPointer<T>& other) {
		object_pointer = other.object_pointer;
		counter = other.counter;
		if (other.counter) {
			counter->sp_count++;
		}
	}
	SmartPointer(WeakPointer<T>& wp) {
		if (wp.object_pointer) {
			object_pointer = wp.object_pointer;
			counter = wp.counter;
			counter->sp_count++;
		}
		else {
			object_pointer = nullptr;
			counter = nullptr;
		}
	}
	SmartPointer(SmartPointer<T>&& other) noexcept{
		object_pointer = other.object_pointer;
		counter = other.counter;
		other.object_pointer = nullptr;
		other.counter = nullptr;
	}
	~SmartPointer() {
		Release();
	}
	
	SmartPointer<T>& operator=(const SmartPointer<T>& other) {
		if (this != &other) {
			Release();
			object_pointer = other.object_pointer;
			counter = other.counter;
			counter->sp_count++;
		}
		return *this;
	}

	void operator=(std::nullptr_t i_nullptr) {
		Release();
		object_pointer = nullptr;
		counter = nullptr;
	}

	T* operator->() {
		if(object_pointer) return object_pointer;
		return nullptr;
	}
	T& operator*() {
		return (*object_pointer);
	}

	operator bool() const noexcept {
		return object_pointer != nullptr;
	}

	// For debug only
	void printCounter() {
		if (counter) {
			std::cout << "Smart Pointer:" << counter->sp_count << std::endl;
			std::cout << "Weak Pointer:" << counter->wp_count << std::endl;
		}
		else {
			std::cout << "No pointers remained." << std::endl;
		}
	}

	void Clean() {
		if (object_pointer) {
			delete object_pointer;
		}
		if (counter) {
			delete counter;
		}
	}

	friend class WeakPointer<T>;
private:
	void Release() {
		if (counter && (--(counter->sp_count) == 0)) {
			if (object_pointer) {
				delete object_pointer;
			}
			object_pointer = nullptr;
			if (counter->wp_count < 1) {
				delete counter;
				counter = nullptr;
			}
		}
	}
	RefCount* counter;
	T* object_pointer;
};

template<class T>
inline bool operator==(SmartPointer<T> const& operand1, SmartPointer<T> const& operand2) {
	if (operand1 && operand2) return *operand1 == *operand2;
	return false;
}

template<class T>
inline bool operator==(SmartPointer<T> const& operand1, std::nullptr_t i_nullptr) {
	return !operand1;
}

template<class T>
inline bool operator==(std::nullptr_t i_nullptrconst, SmartPointer<T> const& operand1) {
	return !operand1;
}

template<class T>
inline bool operator!=(SmartPointer<T> const& operand1, SmartPointer<T> const& operand2) {
	if (operand1 && operand2) return *operand1 != *operand2;
	return false;
}

template<class T>
inline bool operator!=(SmartPointer<T> const& operand1, std::nullptr_t i_nullptrconst) {
	return operand1;
}

template<class T>
inline bool operator!=(std::nullptr_t i_nullptr,SmartPointer<T> const& operand1) {
	return operand1;
}