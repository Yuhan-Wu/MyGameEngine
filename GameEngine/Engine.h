#pragma once

#include "Timer.h"
#include "SmartPointer.h"
#include "JobSystem/JobSystem.h"
#include "JobSystem/IJob.h"
#include "Syncronization/Events.h"
#include "json.hpp"

#include <vector>
#include <map>
#include <string>


namespace Engine {
	namespace CollisionSystem {
		void CheckCollision(float);
	}
	void Start();
	void Tick();
	void Tick(float);
	void Clean();
	
	std::vector<uint8_t> LoadFile(const char* i_pScriptFilename);
	void CreateGameObjects(std::vector<uint8_t> i_pFileContents);
	void CreateGameObjectsWithPosition(std::vector<uint8_t> PlayerData, Point2D initial);
	void CreateGameObjectsWithPositionAndVelocity(std::vector<uint8_t> PlayerData, Point2D initial, Point2D vel);
	void ProcessFileContents(std::vector<uint8_t> i_pFileContents, std::function<void(std::vector<uint8_t>)> i_Processor, Engine::Event* i_pFinishEvent = nullptr);
	void ProcessFileContentsWithPosition(std::vector<uint8_t> i_pFileContents, std::function<void(std::vector<uint8_t>, Point2D)> i_Processor, Point2D initial, Engine::Event* i_pFinishEvent = nullptr);
	void ProcessFileContentsWithPositionAndVelocity(std::vector<uint8_t> i_pFileContents, std::function<void(std::vector<uint8_t>, Point2D, Point2D)> i_Processor, Point2D initial, Point2D vel, Engine::Event* i_pFinishEvent = nullptr);
	void CreateActor(const char* i_pScriptFilename);
	void CreateActorWithPosition(const char* i_pScriptFilename, Point2D initial);
	void CreateActorWithPositionAndVelocity(const char* i_pScriptFilename, Point2D initial, Point2D vel);
	void RemoveActorIfOutOfBorder(int width, int height);

	extern std::map<std::string, std::function<void(SmartPointer<GameObject>&, nlohmann::json&)> >* ControllerCreators;
	extern std::map<std::string, std::function<void(SmartPointer<GameObject>&, nlohmann::json&)> >* ComponentCreators;
	extern std::vector<SmartPointer<GameObject>>* New_GameObject;
	extern std::vector<SmartPointer<GameObject>>* World_GameObject;
}