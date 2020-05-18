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
	~WeakPointer() {
		Release();
	}

	WeakPointer& operator=(WeakPointer<T>& other) {
		if (this != &other) {
			Release();
			object_pointer = other.object_pointer;
			counter = other.counter;
			counter->wp_count++;
		}
		return *this;
	}

	WeakPointer& operator=(SmartPointer<T>& other) {
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
				counter = nullptr;
			}
		}
	}
	RefCount* counter;
	T* object_pointer;
};