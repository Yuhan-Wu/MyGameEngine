#include "pch.h"
#include "AIController.h"

AIController::AIController()
{
	DecayRate = 5;
}

AIController::AIController(SmartPointer<GameObject>& gameObject, float decayRate) {
	sp_GameObject = gameObject;
	DecayRate = decayRate;
}

AIController::~AIController()
{

}

void AIController::ReleaseExtra() {
	sp_GameObject = nullptr;
}