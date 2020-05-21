#pragma once
#include <vector>
#include "Point2D.h"
#include "IGameObjectComponent.h"
#include "IGameObjectController.h"
#include "HeapManager.h"
#include "HeapManagerProxy.h"
#include "SmartPointer.h"

class GameObject
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

	GameObject();
	GameObject(const GameObject&);
	~GameObject();

	static SmartPointer<GameObject> Create(const char* Name, size_t Length, Point2D InitialPosiotion, float Rotation) {
		Point2D* initialPosition = new Point2D(InitialPosiotion.X(), InitialPosiotion.Y());
		GameObject* gameObject = new GameObject(Name, Length, initialPosition, Rotation);
		SmartPointer<GameObject> result(gameObject);
		return result;
	}

	GameObject& operator= (const GameObject&);
	Point2D* position2D = nullptr;
	float RotZ;
	char* name;

	void Attach(IGameObjectController* controller);
	void Attach(IGameObjectComponent* component);

	void BeginUpdate(float delta_time);
	void Update(float delta_time);
	void EndUpdate(float delta_time);

	void ReleaseAll();

	IGameObjectComponent* GetComponent(ComponentType);

protected:
	int nameLength;
	std::vector<IGameObjectComponent*> component_list;
	std::vector<IGameObjectController*> controller_list;

private:
	GameObject(const char* name, size_t length, Point2D* position2D, float rotation);
};