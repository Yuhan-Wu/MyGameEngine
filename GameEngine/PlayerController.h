#pragma once
#include "GameData.h"

class PlayerController:public IGameObjectController
{
public:
	PlayerController();
	PlayerController(SmartPointer<GameObject>& gameObject);
	~PlayerController();

	void ReceiveInput(unsigned int i_VKeyID, bool bWentDown);
	SmartPointer<GameObject> GetPlayer();

	void ReleaseExtra() override;

	std::string Name() override {
		return "Player";
	}

private:
	SmartPointer<GameObject> sp_GameObject;
};