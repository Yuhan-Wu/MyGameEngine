#include "pch.h"
#include "PlayerController.h"
#include "PhysicsInfo.h"
#include "SmartPointer.h"

PlayerController::PlayerController()
{
	
}

PlayerController::PlayerController(SmartPointer<GameObject>& gameObject) {
	sp_GameObject = gameObject;
}

PlayerController::~PlayerController()
{
}

void PlayerController::ReceiveInput(unsigned int i_VKeyID, bool bWentDown) {
	if (i_VKeyID == 0x0044) {
		if (bWentDown) {
			Point2D force(50000, 0);
			reinterpret_cast<PhysicsInfo*>(sp_GameObject->GetComponent(ComponentType::PhysicsInfo))->accelerate(force);
		}
		else {
			reinterpret_cast<PhysicsInfo*>(sp_GameObject->GetComponent(ComponentType::PhysicsInfo))->remove_force();
		}
	} else if (i_VKeyID == 0x0041) {
		if (bWentDown) {
			Point2D force(-50000, 0);
			reinterpret_cast<PhysicsInfo*>(sp_GameObject->GetComponent(ComponentType::PhysicsInfo))->accelerate(force);
		}
		else {
			reinterpret_cast<PhysicsInfo*>(sp_GameObject->GetComponent(ComponentType::PhysicsInfo))->remove_force();
		}
	} else if (i_VKeyID == 0x0053) {
		if (bWentDown) {
			Point2D force(0, -100000);
			reinterpret_cast<PhysicsInfo*>(sp_GameObject->GetComponent(ComponentType::PhysicsInfo))->accelerate(force);
		}
		else {
			reinterpret_cast<PhysicsInfo*>(sp_GameObject->GetComponent(ComponentType::PhysicsInfo))->remove_force();
		}
	} else if (i_VKeyID == 0x0057) {
		if (bWentDown) {
			Point2D force(0, 100000);
			reinterpret_cast<PhysicsInfo*>(sp_GameObject->GetComponent(ComponentType::PhysicsInfo))->accelerate(force);
		}
		else {
			reinterpret_cast<PhysicsInfo*>(sp_GameObject->GetComponent(ComponentType::PhysicsInfo))->remove_force();
		}
	}
}

SmartPointer<GameObject> PlayerController::GetPlayer() {
	return sp_GameObject;
}

void PlayerController::ReleaseExtra() {
	sp_GameObject = nullptr;
}