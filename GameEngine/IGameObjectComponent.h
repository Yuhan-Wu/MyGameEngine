#pragma once

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
	virtual void ReleaseExtra()
	{}
	void SetParent(GameObject* parent) {
		this->parent = parent;
	}
	virtual ComponentType Name() = 0;

protected:
	GameObject* parent = nullptr;
};

#include "GameObject.h"