#pragma once
#include <vector>
#include "Point2D.h"
#include "IGameObjectComponent.h"
#include "IGameObjectController.h"
#include "HeapManager.h"
#include "HeapManagerProxy.h"
#include "SmartPointer.h"

#include <string>

using namespace HeapManagerProxy;

class GameObject
{
public:
	static void* operator new(size_t size) {
		return alloc(HeapManager::GetInstance(), size);
	}
	static void* operator new(size_t size, unsigned int align) {
		return alloc(HeapManager::GetInstance(), size, align);
	}
	static void* operator new[](size_t size) {
		return alloc(HeapManager::GetInstance(), size);
	}

	static void operator delete(void* pointer) {
		free(HeapManager::GetInstance(), pointer);
	}
	static void operator delete[](void* pointer) {
		free(HeapManager::GetInstance(), pointer);
	}

	GameObject();
	GameObject(const GameObject&);
	~GameObject();

	static SmartPointer<GameObject> Create(std::string p_Name, Point2D p_InitialPosiotion, float p_Rotation) {
		GameObject* gameObject = new GameObject(p_Name, p_InitialPosiotion, p_Rotation);
		SmartPointer<GameObject> result(gameObject);
		return result;
	}
	void ReleaseAll();

	void Attach(IGameObjectController* controller);
	void Attach(IGameObjectComponent* component);

	void BeginUpdate(float delta_time);
	void Update(float delta_time);
	void EndUpdate(float delta_time);
	void Render();

	IGameObjectComponent* GetComponent(ComponentType);

	GameObject& operator= (const GameObject&);
	Point2D m_Position2D;
	float m_RotZ;
	std::string m_Name = "Default";

protected:
	std::vector<IGameObjectComponent*> m_Component_list;
	std::vector<IGameObjectController*> m_Controller_list;

private:
	GameObject(std::string name, Point2D position2D, float rotation);
};