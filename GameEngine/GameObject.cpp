#include "pch.h"
#include "GameObject.h"
#include "Engine.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

using namespace std;

GameObject::GameObject()
{
	GameObject::m_Position2D = Point2D(0, 0);
	m_RotZ = 0;
}

GameObject::GameObject(std::string name, Point2D position2D, float rotation) {
	// resize
	if (name.length() > 0) {
		name = name;
	}

	GameObject::m_Position2D = position2D;
	m_RotZ = rotation;
}

GameObject::GameObject(const GameObject& other) {
	this->m_Name = other.m_Name;
	this->m_Position2D = other.m_Position2D;
	m_RotZ = other.m_RotZ;
}

GameObject::~GameObject()
{
}

GameObject& GameObject::operator= (const GameObject& other) {
	this->m_Name = other.m_Name;
	this->m_Position2D = other.m_Position2D;
	return *this;
}

void GameObject::Attach(IGameObjectController* controller) {
	if (controller) {
		m_Controller_list.push_back(controller);
		controller->SetParent(this);
	}
}

void GameObject::Attach(IGameObjectComponent* component) {
	if (component) {
		m_Component_list.push_back(component);
		component->SetParent(this);
	}
}

void GameObject::ReleaseAll() {
	while (m_Controller_list.size() != 0) {
		IGameObjectController* controller = m_Controller_list.back();
		m_Controller_list.pop_back();
		controller->ReleaseExtra();
		delete controller;
	}

	while (m_Component_list.size() != 0) {
		IGameObjectComponent* component = m_Component_list.back();
		m_Component_list.pop_back();
		component->ReleaseExtra();
		delete component;
	}
}

void GameObject::BeginUpdate(float delta_time)
{
	size_t count = m_Controller_list.size();
	for (size_t i = 0; i < count; i++)
	{
		assert(m_Controller_list[i]);
		m_Controller_list[i]->BeginUpdate(delta_time);
	}

	count = m_Component_list.size();
	for (size_t i = 0; i < count; i++)
	{
		assert(m_Component_list[i]);
		m_Component_list[i]->BeginUpdate(delta_time);
	}
}

void GameObject::Update(float delta_time)
{
	size_t count = m_Controller_list.size();
	for (size_t i = 0; i < count; i++)
	{
		assert(m_Controller_list[i]);
		m_Controller_list[i]->Update(delta_time);
	}

	count = m_Component_list.size();
	for (size_t i = 0; i < count; i++)
	{
		assert(m_Component_list[i]);
		m_Component_list[i]->Update(delta_time);
	}
}
void GameObject::EndUpdate(float delta_time)
{
	size_t count = m_Controller_list.size();
	for (size_t i = 0; i < count; i++)
	{
		assert(m_Controller_list[i]);
		m_Controller_list[i]->EndUpdate(delta_time);
	}

	count = m_Component_list.size();
	for (size_t i = 0; i < count; i++)
	{
		assert(m_Component_list[i]);
		m_Component_list[i]->EndUpdate(delta_time);
	}
}
void GameObject::Render() {
	size_t count = m_Component_list.size();
	for (size_t i = 0; i < count; i++)
	{
		assert(m_Component_list[i]);
		m_Component_list[i]->Render();
	}
}

IGameObjectComponent* GameObject::GetComponent(ComponentType p_RequiredType) {
	for (IGameObjectComponent* component : m_Component_list) {
		if (component->Name() == p_RequiredType) {
			return component;
		}
	}
	return nullptr;
}