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
		void CreateGameObjects(std::vector<uint8_t> p_FileContents);
		void CreateGameObjects(std::vector<uint8_t> p_PlayerData, Point2D p_Initial);
		void CreateGameObjects(std::vector<uint8_t> p_PlayerData, Point2D p_Initial, Point2D p_Vel);
		
		void ProcessFileContents(std::vector<uint8_t> p_FileContents, std::function<void(std::vector<uint8_t>)> p_Processor, Engine::Event* p_FinishEvent = nullptr);
		void ProcessFileContents(std::vector<uint8_t> p_FileContents, std::function<void(std::vector<uint8_t>, Point2D)> p_Processor, Point2D p_Initial, Engine::Event* p_FinishEvent = nullptr);
		void ProcessFileContents(std::vector<uint8_t> p_FileContents, std::function<void(std::vector<uint8_t>, Point2D, Point2D)> p_Processor, Point2D p_Initial, Point2D p_Vel, Engine::Event* p_FinishEvent = nullptr);
		
		void CreateActor(const char* p_ScriptFilename);
		void CreateActor(const char* p_ScriptFilename, Point2D p_Initial);
		void CreateActor(const char* p_ScriptFilename, Point2D p_Initial, Point2D p_Vel);
	}

	namespace Game {
		void RemoveActorIfOutOfBorder(int width, int height);
	}

	extern std::map<std::string, std::function<void(SmartPointer<GameObject>&, nlohmann::json&)> >* ControllerCreators;
	extern std::map<std::string, std::function<void(SmartPointer<GameObject>&, nlohmann::json&)> >* ComponentCreators;
	extern std::vector<SmartPointer<GameObject>>* New_GameObject;
	extern std::vector<SmartPointer<GameObject>>* World_GameObject;
}