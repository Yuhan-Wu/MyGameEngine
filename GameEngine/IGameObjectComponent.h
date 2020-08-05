#pragma once

#include <iostream> 
#include "HeapManager.h"
#include "HeapManagerProxy.h"
#include "ComponentType.h"

class GameObject;

class IGameObjectComponent
{
public:
	static void* operator new(size_t size) {
		using namespace HeapManagerProxy;
		return alloc(HeapManager::GetInstance(), size);
	}
	static void* operator new(size_t size, unsigned int align) {
		using namespace HeapManagerProxy;
		return alloc(HeapManager::GetInstance(), size, align);
	}
	static void* operator new[](size_t size) {
		using namespace HeapManagerProxy;
		return alloc(HeapManager::GetInstance(), size);
	}

		static void operator delete(void* pointer) {
		using namespace HeapManagerProxy;
		free(HeapManager::GetInstance(), pointer);
	}
	static void operator delete[](void* pointer) {
		using namespace HeapManagerProxy;
		free(HeapManager::GetInstance(), pointer);
	}

	virtual void BeginUpdate(float delta_time)
	{}
	virtual void Update(float delta_time)
	{}
	virtual void EndUpdate(float delta_time)
	{}
	virtual void Render()
	{}

	virtual void ReleaseExtra()
	{}
	void SetParent(GameObject* parent) {
		this->m_Parent = parent;
	}
	virtual ComponentType Name() = 0;

protected:
	typedef void(*FunctionPointer)(float);
	GameObject* m_Parent = nullptr;
};

#include "GameObject.h"