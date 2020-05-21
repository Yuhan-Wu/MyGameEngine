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
	char defaultname[8] = { 'D','E','F','A','U','L','T','\0' };
	nameLength = 8;
	GameObject::name = (char*)malloc(nameLength);
#pragma warning(suppress:6387)
	memcpy(GameObject::name, defaultname, (nameLength) * sizeof(char));
	GameObject::position2D = new Point2D(0, 0);
	RotZ = 0;
}

GameObject::GameObject(const char* name, size_t length, Point2D* position2D, float rotation) {
	// resize
	if (length > 0) {
		nameLength = static_cast<int>(length);
		GameObject::name = (char*)malloc(length);
		memcpy(GameObject::name, name, (length) * sizeof(char));
	}
	else {
		char defaultname[8] = { 'D','E','F','A','U','L','T','\0' };
		nameLength = 8;
		GameObject::name = (char*)malloc(nameLength);
#pragma warning(suppress:6387)
		memcpy(GameObject::name, defaultname, (nameLength) * sizeof(char));
	}

	GameObject::position2D = new Point2D();
	if (position2D) {
		*(GameObject::position2D) = *position2D;
		delete position2D;
	}
	RotZ = rotation;
}

GameObject::GameObject(const GameObject& other) {
	this->name = other.name;
	this->nameLength = other.nameLength;
	this->position2D = new Point2D();
	this->position2D = other.position2D;
	RotZ = other.RotZ;
}

GameObject::~GameObject()
{
	free(name);
	name = nullptr;
	if (position2D) {
		delete position2D;
		position2D = nullptr;
	}
}

GameObject& GameObject::operator= (const GameObject& other) {
	this->name = other.name;
	this->nameLength = other.nameLength;
	this->position2D = other.position2D;
	return *this;
}

void GameObject::Attach(IGameObjectController* controller) {
	if (controller) {
		controller_list.push_back(controller);
		controller->SetParent(this);
	}
}

void GameObject::Attach(IGameObjectComponent* component) {
	if (component) {
		component_list.push_back(component);
		component->SetParent(this);
	}
}

void GameObject::ReleaseAll() {
	while (controller_list.size() != 0) {
		IGameObjectController* controller = controller_list.back();
		controller_list.pop_back();
		controller->ReleaseExtra();
		delete controller;
	}
}

void GameObject::BeginUpdate(float delta_time)
{
	size_t count = controller_list.size();
	for (size_t i = 0; i < count; i++)
	{
		assert(controller_list[i]);
		controller_list[i]->BeginUpdate(delta_time);
	}
}

void GameObject::Update(float delta_time)
{
	size_t count = controller_list.size();
	for (size_t i = 0; i < count; i++)
	{
		assert(controller_list[i]);
		controller_list[i]->Update(delta_time);
	}
}
void GameObject::EndUpdate(float delta_time)
{
	size_t count = controller_list.size();
	for (size_t i = 0; i < count; i++)
	{
		assert(controller_list[i]);
		controller_list[i]->EndUpdate(delta_time);
	}
}

IGameObjectComponent* GameObject::GetComponent(ComponentType p_RequiredType) {
	for (IGameObjectComponent* component : component_list) {
		if (component->Name() == p_RequiredType) {
			return component;
		}
	}
}