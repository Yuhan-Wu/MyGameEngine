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
	
	namespace FileProcess {
		std::vector<uint8_t> LoadFile(const char* p_ScriptFilename);
		void CreateGameObjects(std::vector<uint8_t> p_FileContents, std::string p_ID);
		void CreateGameObjects(std::vector<uint8_t> p_PlayerData, std::string p_ID, Point2D p_Initial);
		void CreateGameObjects(std::vector<uint8_t> p_PlayerData, std::string p_ID, Point2D p_Initial, Point2D p_Vel);
		
		void ProcessFileContents(std::vector<uint8_t> p_FileContents, std::function<void(std::vector<uint8_t>, std::string)> p_Processor, std::string p_ID, Engine::Event* p_FinishEvent = nullptr);
		void ProcessFileContents(std::vector<uint8_t> p_FileContents, std::function<void(std::vector<uint8_t>, std::string, Point2D)> p_Processor, std::string p_ID, Point2D p_Initial, Engine::Event* p_FinishEvent = nullptr);
		void ProcessFileContents(std::vector<uint8_t> p_FileContents, std::function<void(std::vector<uint8_t>, std::string, Point2D, Point2D)> p_Processor, std::string p_ID, Point2D p_Initial, Point2D p_Vel, Engine::Event* p_FinishEvent = nullptr);
		
		SmartPointer<GameObject> CreateActor(const char* p_ScriptFilename);
		SmartPointer<GameObject> CreateActor(const char* p_ScriptFilename, Point2D p_Initial);
		SmartPointer<GameObject> CreateActor(const char* p_ScriptFilename, Point2D p_Initial, Point2D p_Vel);
	}

	namespace Game {
		void RemoveActorIfOutOfBorder(int width, int height);
	}

	extern std::map<std::string, std::function<void(SmartPointer<GameObject>&, nlohmann::json&)> >* ControllerCreators;
	extern std::map<std::string, std::function<void(SmartPointer<GameObject>&, nlohmann::json&)> >* ComponentCreators;
	extern std::map<std::string, SmartPointer<GameObject>>* New_GameObject;
	extern std::vector<SmartPointer<GameObject>>* World_GameObject;
}