#pragma once
#include "SmartPointer.h"
#include "GameObject.h"

class AIController:public IGameObjectController
{
public:
	AIController();
	AIController(SmartPointer<GameObject>& gameObject, float decayRate);
	~AIController();

	void ReleaseExtra() override;

	std::string Name() override{
		return "AI";
	}

private:
	SmartPointer<GameObject> sp_GameObject;
	float DecayRate;
};